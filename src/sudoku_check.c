#include <stdio.h>
#include <assert.h>

#include "checker_io.h"
#include "checker_sd.h"

main() {
  sd_check_t sd;
  scan_sudoku(&sd);
  print_sudoku(sd);
  switch(check_sudoku(sd)) {
    case INVALID:
      puts("INVALID");
    break;
    case INCOMPLETE:
      puts("INCOMPLETE");
    break;
    case COMPLETE:
      puts("COMPLETE");
    break;
  }
  clear_sd(sd);
}
