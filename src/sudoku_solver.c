#include <stdio.h>

#include "solver_io.h"
#include "solver.h"

main() {
  sudoku_t *s = scan_sudoku(s);
  if(s == NULL) {
    printf("UNSOLVABLE\n");
    return 0;
  }
  solve_sudoku(&s);
  switch(s->res) {
    case INVALID:
      printf("UNSOLVABLE\n");
    break;
    case COMPLETE:
      print_sudoku(s);
    break;
    case MULTIPLE:
      printf("MULTIPLE\n");
    break;
  }
  free_sudoku(&s);
}
