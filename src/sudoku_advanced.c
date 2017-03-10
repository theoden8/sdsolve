#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "advanced.h"

void run_sudoku() {
  sz_t n;
scan:
  scanf(" %u", &n);
  val_t vals[n*n*n*n];
  for(sz_t i = 0; i < n*n*n*n; ++i) {
    int sc = scanf(" %hhu", &vals[i]);
    assert(sc == 1);
    assert(vals[i] <= n*n);
  }
solve:;
  sd_t *s = make_sd(n, vals);
  RESULT res = solve_sd(s);
  if(res != COMPLETE)goto show_res;
print:;
  for(sz_t y = 0; y < s->ne2; ++y) {
    for(sz_t x = 0; x < s->ne2; ++x) {
      int t = s->table[y * s->ne2 + x];
      int len = t ? 0 : 1;
      while(t){t/=10;++len;}
      for(int i=0;i<3-len;++i)putchar(' ');
      printf("%d", s->table[y * s->ne2 + x]);
    }
    putchar('\n');
  }
show_res:
  switch(res) {
    case MULTIPLE: puts("MULTIPLE"); break;
    case INVALID: puts("UNSOLVABLE"); break;
    case COMPLETE:break;
  }
  free_sd(s);
}

int main() {
  run_sudoku();
}
