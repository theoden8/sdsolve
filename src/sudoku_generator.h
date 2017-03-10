#ifndef GENERATE_SUDOKU_H_EA4NV8BT
#define GENERATE_SUDOKU_H_EA4NV8BT

#include <stdint.h>

#include "advanced.h"

typedef struct {
  sz_t n, ne2, ne4;
  val_t *table;
  RESULT status;
  sz_t no_vals;
} sdgen_t;

void seed_rng();
sdgen_t create_empty_table(sz_t n);
void free_table(sdgen_t s);
void print_table(sdgen_t s);
bool solve(sdgen_t *s);
void complete(sdgen_t *s);
void set_random(sdgen_t *s);
void unset_random(sdgen_t *s);

#endif
