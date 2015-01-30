#ifndef NEW_BUILD_H
#define NEW_BUILD_H

#include "bilge.h"

void mark_bilgefiles(struct all_targets *all);
void mark_all(struct all_targets *all);
void mark_rule(struct all_targets *all, struct rule *r);

void build_marked(struct all_targets *all, const char *root_);
void check_for_impossibilities(struct all_targets *all, const char *_root);

void build_continual(const char *root_);

void summarize_build_results(struct all_targets *all);

#endif