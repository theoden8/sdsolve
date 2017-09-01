#ifndef SOLVE_SUDOKU_H_0QMSPMYJ
#define SOLVE_SUDOKU_H_0QMSPMYJ

#include <stdbool.h>

#include "solver_sd.h"

void solve_sudoku(sudoku_t **s);
bool is_completed_sudoku(const sudoku_t *s);
bool is_valid_sudoku(const sudoku_t *s);
void recurse_sudoku(sudoku_t **s, sz_t pos);
sz_t find_best_recurse_point(const sudoku_t *s);
void sudoku_iterate_known_values(sudoku_t *s);
void set_sudoku_value(sudoku_t *s, sz_t pos, val_t x);

void unset_possible_val_d(sudoku_t *s, sz_t pos, val_t t);
static void eliminate_value(sudoku_t *s, sz_t pos);

#endif
