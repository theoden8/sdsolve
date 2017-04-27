#ifndef GENERATE_SUDOKU_H_EA4NV8BT
#define GENERATE_SUDOKU_H_EA4NV8BT

#include <stdint.h>
#include <stdbool.h>

#include "advanced.h"

typedef struct {
  sz_t n, ne2, ne4;
  val_t *table;
  RESULT status;
  sz_t no_vals;
} sdgen_t;

void seed_rng();
sdgen_t sdgen_init(sz_t n);
void sdgen_free(sdgen_t s);
void print_table(sdgen_t s);
RESULT solve(sdgen_t *s);
void complete(sdgen_t *s);
void sd_fill_box(sdgen_t *s, sz_t i);
bool set_random(sdgen_t *s);
void unset_random(sdgen_t *s);
bool try_unset(sdgen_t *s, sz_t idx);

static void print_arr(sz_t *arr, sz_t len);
static void ord_arr(sz_t *arr, sz_t len);
static void shuffle_arr(sz_t *arr, sz_t len);
static void shift_arr(sz_t *arr, sz_t idx, sz_t *len);

#endif
