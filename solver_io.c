#include <stdio.h>
#include <assert.h>

#include "solver_io.h"
#include "solver_sd.h"
#include "solver.h"

sudoku_t *scan_sudoku() {
  size_t n = 2;
  scanf(" %lu", &n);
  sudoku_t *s = alloc_sudoku(n);
  for(sz_t pos = 0; pos < s->n_e4; ++pos) {
    val_t value = 0;
    int sc = scanf(" %hhu", &value);
    if(sc != 1 || value > s->n_e2) {
      free_sudoku(&s);
      return NULL;
    }
    set_sudoku_val_d(s, pos, value);
  }
  return s;
}

static int chrlen(val_t val) {
  if(!val)
    return 1;
  int len = 0;
  while(val)
    val /= 10,
    ++len;
  return len;
}

void print_sudoku(const sudoku_t *s) {
  int len = 3;
  for(size_t y = 0; y < s->n_e2; ++y) {
    for(size_t x = 0; x < s->n_e2; ++x) {
      size_t pos = make_pos_xy(s, x, y);
      int plen = len - chrlen(get_sudoku_val(s, pos));
      for(char c = 0; c < plen; ++c)
        putchar(' ');
      printf("%d", get_sudoku_val(s, pos));
    }
    printf("\n");
  }
}

void copy_sudoku(sudoku_t **restrict dst, const sudoku_t *restrict src) {
  *dst = cpy_sudoku(src);
}

const char *sudoku_res_to_string(sudoku_res_t r) {
  switch(r) {
    case INVALID: return "INVALID";
    case MULTIPLE: return "MULTIPLE";
    case INCOMPLETE: return "INCOMPLETE";
    case COMPLETE: return "COMPLETE";
  }
}
