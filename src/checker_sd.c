#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "checker_sd.h"

// allocate a sudoku board on stack
sd_check_t alloc_sd(sz_t n) {
  sd_check_t sd;
  sd.n = n;
  sd.ne2 = sd.n * sd.n;
  sd.ne4 = sd.ne2 * sd.ne2;
  sd.board = malloc(sizeof(int) * sd.ne4);
  assert(sd.board != NULL);
  return sd;
}

// deallocate the malloced bit
void clear_sd(sd_check_t sd) {
  free(sd.board);
}

checker_res check_list(sz_t n, int *a) {
  char values[n*n];
  memset(values, 0x00, n * sizeof(char));
  checker_res ret = COMPLETE;
  for(sz_t i = 0; i < n; ++i) {
    if(a[i] == 0) {
      assert(ret != INVALID);
      if(ret == COMPLETE)
        ret = INCOMPLETE;
      continue;
    }
    if(values[a[i] - 1])
      return INVALID;
    else
      values[a[i] - 1] = true;
  }
  return ret;
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
checker_res check_sudoku(sd_check_t sd) {
  int arrays[sd.ne2 * CONSTRSIZE];
  checker_res res = COMPLETE;
  for(sz_t i = 0; i < sd.ne2; ++i) {
    for(sz_t j = 0; j < sd.ne2; ++j) {
      arrays[j + sd.ne2*ROW] = sd.board[(sd.n * (i / sd.n) + (j / sd.n)) * sd.ne2 +  (sd.n * (i % sd.n) + (j % sd.n))];
      arrays[j + sd.ne2*COL] = sd.board[i * sd.ne2 + j];
      arrays[j + sd.ne2*BOX] = sd.board[j * sd.ne2 + i];
    }
    res = MIN(res, check_list(sd.ne2, arrays + sd.ne2*ROW));
    if(res == INVALID)
      return res;
    res = MIN(res, check_list(sd.ne2, arrays + sd.ne2*COL));
    if(res == INVALID)
      return res;
    res = MIN(res, check_list(sd.ne2, arrays + sd.ne2*BOX));
    if(res == INVALID)
      return res;
  }
  return res;
}
