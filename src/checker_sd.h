#ifndef SUDOKU_H_HDWXQIMH
#define SUDOKU_H_HDWXQIMH

#include <stdint.h>

typedef uint32_t sz_t;

typedef struct _sd_check_t {
  sz_t n, ne2, ne4;
  int *board;
} sd_check_t;

sd_check_t alloc_sd(sz_t n);
void clear_sd(sd_check_t sd);

typedef enum { INVALID, INCOMPLETE, COMPLETE } checker_res;
typedef enum { ROW, COL, BOX, CONSTRSIZE } CONSTR;

checker_res check_list(sz_t n, int *a);
checker_res check_sudoku(sd_check_t sd);

#endif
