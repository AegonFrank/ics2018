#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char what[100];

} WP;

WP* new_wp();

WP* find_wp(int NO);

void free_wp(WP *wp);

void print_wp();

#endif
