#ifndef SUDOKU_IO_H_EPSIIKLT
#define SUDOKU_IO_H_EPSIIKLT

#include "solver_sd.h"

sudoku_t *scan_sudoku();
void print_sudoku(const sudoku_t *s);
void copy_sudoku(sudoku_t **restrict dst, const sudoku_t *restrict src);
const char *sudoku_res_to_string(sudoku_res_t r);

#endif
