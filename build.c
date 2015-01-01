#include "bilge.h"
#include "lib/bigbrother.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <string.h>

static struct target *create_target_with_stat(struct all_targets **all,
                                              const char *path) {
  struct target *t = create_target(all, path);
  if (!t->last_modified) {
    struct stat st;
    if (stat(t->path, &st)) return 0;
    t->size = st.st_size;
    t->last_modified = st.st_mtime;
  }
  return t;
}

struct rule *run_rule(struct all_targets **all, struct rule *r) {
  struct rule *out = r; //create_rule(r->command, r->working_directory);
  listset *read_set = 0, *written_set = 0, *deleted_set = 0;
  const char **args = malloc(4*sizeof(char *));
  args[0] = "/bin/sh";
  args[1] = "-c";
  args[2] = r->command;
  args[3] = 0;
  printf("%s\n", r->command);
  int ret = bigbrother_process(r->working_directory,
                               (char **)args,
                               &read_set, &written_set, &deleted_set);
  free(args);
  if (ret != 0) {
    free_listset(read_set);
    free_listset(written_set);
    free_listset(deleted_set);
    return 0;
  }

  listset *s = read_set;
  while (s != NULL) {
    struct target *t = create_target_with_stat(all, s->path);
    if (!t) error(1, errno, "Unable to stat file %s", t->path);
    add_input(out, t);
    s = s->next;
  }

  for (int i=0;i<out->num_outputs;i++) {
    /* The following handles the case where we have a command that
       doesn't actually write to one of its "outputs." */
    create_target_with_stat(all, out->outputs[i]->path);
  }

  s = written_set;
  while (s != NULL) {
    struct target *t = lookup_target(*all, s->path);
    if (t) {
      t->last_modified = 0;
      t->size = 0;
    }
    t = create_target_with_stat(all, s->path);
    if (!t) error(1, errno, "Unable to stat file %s", t->path);
    t->rule = out;
    add_output(out, t);
    s = s->next;
  }

  free_listset(read_set);
  free_listset(written_set);
  free_listset(deleted_set);
  return out;
}

void determine_rule_cleanliness(struct all_targets **all, struct rule *r,
                                int *num_to_build) {
  if (!r) return;
  if (r->status != unknown) return;
  for (int i=0;i<r->num_inputs;i++) {
    if (r->inputs[i]->rule) {
      if (r->inputs[i]->rule->status == unknown)
        determine_rule_cleanliness(all, r->inputs[i]->rule, num_to_build);
      if (r->inputs[i]->rule->status == dirty) {
        printf("::: %s :::\n", r->command);
        printf(" - dirty because %s needs to be rebuilt.\n",
               r->inputs[i]->path);
        r->status = dirty;
        *num_to_build += 1;
        return;
      }
    }
    if (r->input_times[i]) {
      if (!create_target_with_stat(all, r->inputs[i]->path) ||
          r->input_times[i] != r->inputs[i]->last_modified ||
          r->input_sizes[i] != r->inputs[i]->size) {
        printf("::: %s :::\n", r->command);
        printf(" - dirty because %s has wrong input time.\n",
               r->inputs[i]->path);
        r->status = dirty;
        *num_to_build += 1;
        return; /* The file is out of date. */
      }
    } else {
      printf("::: %s :::\n", r->command);
      printf(" - dirty because #%d %s has no input time.\n", i, r->inputs[i]->path);
      r->status = dirty;
      *num_to_build += 1;
      return; /* The file hasn't been built. */
    }
  }
  for (int i=0;i<r->num_outputs;i++) {
    if (r->output_times[i]) {
      if (!create_target_with_stat(all, r->outputs[i]->path) ||
          r->output_times[i] != r->outputs[i]->last_modified ||
          r->output_sizes[i] != r->outputs[i]->size) {
        printf("::: %s :::\n", r->command);
        printf(" - dirty because %s has wrong output time.\n",
               r->outputs[i]->path);
        printf("   compare times %ld with %ld\n",
               r->outputs[i]->last_modified, r->output_times[i]);
        r->status = dirty;
        *num_to_build += 1;
        return; /* The file is out of date. */
      }
    } else {
      printf("::: %s :::\n", r->command);
      printf(" - dirty because %s has no output time.\n", r->outputs[i]->path);
      r->status = dirty;
      *num_to_build += 1;
      return; /* The file hasn't been built. */
    }
  }
  if (r->status == unknown) r->status = clean;
}

void build_rule_plus_dependencies(struct all_targets **all, struct rule *r,
                                  int *num_to_build, int *num_built) {
  if (!r) return;
  if (r->status == unknown) {
    determine_rule_cleanliness(all, r, num_to_build);
  }
  if (r->status == dirty) {
    for (int i=0;i<r->num_inputs;i++)
      build_rule_plus_dependencies(all, r->inputs[i]->rule, num_to_build, num_built);

    *num_built += 1;
    printf("%d/%d: ", *num_built, *num_to_build);
    if (!run_rule(all, r))
      error_at_line(1, 0, r->bilgefile_path, r->bilgefile_linenum,
                    "Error running \"%s\"", r->command);
    r->status = built;

    char *donefile = done_name(r->bilgefile_path);
    FILE *f = fopen(donefile, "w");
    if (!f) error(1,errno,"oopse");
    fprint_bilgefile(f, *all, r->bilgefile_path);
    fclose(f);
    free(donefile);
  }
}

void build_all(struct all_targets **all) {
  bool done = false;
  struct all_targets *tt = *all;
  while (tt) {
    if (tt->t->rule) tt->t->rule->status = unknown;
    tt = tt->next;
  }
  while (!done) {
    int num_to_build = 0, num_built = 0;
    tt = *all;
    while (tt) {
      determine_rule_cleanliness(all, tt->t->rule, &num_to_build);
      tt = tt->next;
    }
    bool got_new_bilgefiles = false;
    tt = *all;
    while (tt) {
      if (tt->t->rule && tt->t->rule->status == dirty) {
        int len = strlen(tt->t->path);
        if (len >= 6 && !strcmp(tt->t->path+len-6, ".bilge")) {
          /* This is a dirty .bilge file, so we need to build it! */
          build_rule_plus_dependencies(all, tt->t->rule, &num_to_build, &num_built);
          read_bilge_file(all, tt->t->path);
          got_new_bilgefiles = true;
          break;
        }
      }
      if (tt->t->rule && tt->t->rule->status == clean) {
        int len = strlen(tt->t->path);
        if (len >= 6 && !strcmp(tt->t->path+len-6, ".bilge")) {
          /* This is a dirty .bilge file, so we need to build it! */
          read_bilge_file(all, tt->t->path);
          got_new_bilgefiles = true;
          tt->t->rule->status = built;
          break;
        }
      }
      tt = tt->next;
    }
    if (got_new_bilgefiles) continue;
    tt = *all;
    while (tt) {
      build_rule_plus_dependencies(all, tt->t->rule, &num_to_build, &num_built);
      tt = tt->next;
    }
    done = true;
  }
}