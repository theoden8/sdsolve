#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "checker_io.h"

void scan_sudoku(sd_check_t *sd) {
  sz_t n;
  scanf(" %hu", &n);
  *sd = alloc_sd(n);
  for(sz_t i = 0; i < sd->ne4; ++i) {
    int sc = scanf(" %d", &sd->board[i]);
    if(sc != 1) {
      fputs("error: could not read a symbol", stderr);
      exit(1);
    }
  }
}

static int chrlen(int x) {
  if(x == 0)
    return 1;
  int len = 0;
  while(x) {
    x /= 10;
    ++len;
  }
  return len;
}


void print_sudoku(sd_check_t sd) {
  for(sz_t y = 0; y < sd.ne2; ++y) {
    for(sz_t x = 0; x < sd.ne2; ++x) {
      int len = chrlen(sd.board[y * sd.ne2 + x]);
      for(int i = 0; i < 3 - len; ++i)
        putchar(' ');
      printf("%d", sd.board[y * sd.ne2 + x]);
    }
    putchar('\n');
  }
}

sd_check_t copy_sudoku(sd_check_t sd) {
  sd_check_t sd2 = alloc_sd(sd.n);
  memcpy(sd2.board, sd.board, sizeof(int) * sd.ne4);
  return sd2;
}
