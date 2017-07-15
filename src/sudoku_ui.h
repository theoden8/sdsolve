#ifndef SUDOKU_UI_H_X7BVHNDU
#define SUDOKU_UI_H_X7BVHNDU

#include <stdint.h>

#include "algx.h"

extern const RESULT INCOMPLETE;

typedef enum { NORMAL, NEGATIVE, FAILURE, SUCCESS, WARNING } COLOR;
typedef struct _xybuf { int x, y; int val; } xybuf;

typedef struct _ui_sudoku_t {
  RESULT res;
  bool compiled;
  int n, ne2, ne4;
  uint8_t *table;
} ui_sudoku_t;

void display(const ui_sudoku_t *s, int r, int c);
void compile_sudoku(ui_sudoku_t *s);
void solve_position(ui_sudoku_t *s, xybuf buf);
int start_ui(ui_sudoku_t *s);

#endif
