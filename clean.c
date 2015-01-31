#include "fac.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static const char *pretty_path(const char *path) {
  int len = strlen(root);
  if (path[len] == '/' && !memcmp(path, root, len)) {
    return path + len + 1;
  }
  return path;
}

void clean_all(struct all_targets *all) {
  for (struct target *t = (struct target *)all->t.first; t; t = (struct target *)t->e.next) {
    t->status = unknown;
    if (is_facfile(t->path)) {
      char *donef = done_name(t->path);
      printf("rm %s\n", donef);
      unlink(donef);
      free(donef);
    }
  }
  for (struct rule *r = (struct rule *)all->r.first; r; r = (struct rule *)r->e.next) {
    for (int i=0;i<r->num_outputs;i++) {
      if (r->outputs[i]->status == unknown) {
        printf("rm %s\n", pretty_path(r->outputs[i]->path));
        unlink(r->outputs[i]->path);
      }
    }
  }
}
