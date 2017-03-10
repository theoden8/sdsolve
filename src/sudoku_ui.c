#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <alloca.h>

#include <ncurses.h>

#include "advanced.h"
#include "sudoku_ui.h"

// note: it is impossible to redirect the file using <
// because the EOF symbol will also be passed by all getch() functions, and
// therefore the UI program will stop working.

const RESULT INCOMPLETE = -1;

static const char *gets_result(const ui_sudoku_t *s);

static int chrlen(int x){if(x == 0)return 1;int len=0;while(x){x/=10;++len;}return len;} //get ints len in string repr
void display(const ui_sudoku_t *s, int r, int c) {
  bkgd(COLOR_PAIR(NORMAL)); // changes the background
  int cols, rows;
  getmaxyx(stdscr, rows, cols); // acquires screen height and width
  const TOP = rows * 0.05 + 2, LEFT = cols * 0.03;

  move(2, LEFT); // move sets the position of the cursor (where printw and addch will write)
  printw("Press ESC to leave.");

print_board:;
  int len = chrlen(s->ne2) + 2 + 1; // character length of a cell in the user interface
  attron(COLOR_PAIR(NORMAL));
  int top = TOP; // y coordinate of where to start writing
  for(int y = 0; y < s->ne2; ++y) {
    move(top, LEFT);
    if(y && !(y % s->n)) {
      for(int x = 0; x < s->ne2; ++x) {
        if(x && !(x % s->n))
          addch(ACS_PLUS), // this is + -like symbol
          addch(ACS_HLINE); // this is | -like symbol
        for(int i = 0; i < len; ++i)
          addch(ACS_HLINE); // this is - -like symbol
      }
      ++top;
      move(top, LEFT);
    }
    for(int x = 0; x < s->ne2; ++x) {
      attron(COLOR_PAIR(NORMAL)); // sets the color used for printing text
      if(x && !(x % s->n))
        /* printw("| "); */
        addch(ACS_VLINE),
        addch(' ');
      const uint8_t t = s->table[y * s->ne2 + x];
      int plen = len;
      int p = chrlen(t);
      if(r == y && c == x) { // [num]
        if(t) {
          printw("[%hhu]", t);
        } else {
          p = chrlen(s->ne2);
          addch('[');
          for(int i = 0; i < p; ++i)
            addch(ACS_BOARD);
          addch(']');
        }
      } else {
        if(!t) // "   "
          printw("   ", t);
        else // " num "
          printw(" %hhu ", t);
      }
      plen -= p + 2;
      for(int i = 0; i < plen; ++i)
        addch(' ');
    }
    ++top;
  }
print_statusbar:;
  move(top + 2, LEFT);
  attron(A_BOLD);
  //set_statusbar_message
  if(s->compiled) {
    COLOR color = NORMAL;
    switch(s->res) {
      case INVALID:
        color = FAILURE;
      break;
      case COMPLETE:
        color = SUCCESS;
      break;
      case MULTIPLE:
        color = WARNING;
      break;
    }
    attron(COLOR_PAIR(color));
  } else attron(COLOR_PAIR(NORMAL));
  //actually_print_it
  len = printw("[ %s ]", gets_result(s));
  attron(COLOR_PAIR(NORMAL));
  for(int i = 0; i < 20 - len; ++i)
    addch(' ');
  refresh();
}

// set the result
void compile_sudoku(ui_sudoku_t *s) {
  s->compiled = true;
  sd_t *solver = make_sd(s->n, s->table);
  s->res = solve_sd(solver);
  free_sd(solver);
}

void solve_position(ui_sudoku_t *s, xybuf buf) {
  if(s->compiled && s->res != COMPLETE)
    return;
  sd_t *solver = make_sd(s->n, s->table);
  RESULT res = solve_sd(solver);
  if(!s->compiled) {
    s->res = res;
    s->compiled = true;
    if(res != COMPLETE)
      return;
  }
  int pos = buf.y * s->ne2 + buf.x;
  s->table[pos] = solver->table[pos];
  free_sd(solver);
}

static const char *gets_result(const ui_sudoku_t *s) {
  if(s->res == INCOMPLETE)return "INCOMPLETE";
  switch(s->res) {
    case INVALID: return "INVALID";
    case COMPLETE: return "VALID";
    case MULTIPLE: return "MULTIPLE";
  }
}

static void update_cell(ui_sudoku_t *s, xybuf buf) {
  if(buf.val == -1)
    return;
  assert(buf.val >= 0 && buf.val <= s->ne2);
  uint8_t idx = buf.y * s->ne2 + buf.x;
  if(s->table[idx] != buf.val) {
    s->table[idx] = buf.val;
    s->compiled = false;
  }
}

int start_ui(ui_sudoku_t *s) {
  int ch;
  initscr();
  start_color();
  init_pair(NORMAL,  COLOR_WHITE, COLOR_BLACK);
  init_pair(NEGATIVE,COLOR_BLACK, COLOR_WHITE);
  init_pair(FAILURE, COLOR_RED, COLOR_BLACK);
  init_pair(SUCCESS, COLOR_GREEN, COLOR_BLACK);
  init_pair(WARNING, COLOR_YELLOW, COLOR_BLACK);

  cbreak();
  noecho(); // do not show typed characters
  keypad(stdscr, TRUE); // attack keyboard to stdscr

  refresh();
  box(stdscr, 0, 0); // create a box in stdscr
  refresh();

  // cursor, basically, but called buf because it keeps the value we want to set
  xybuf buf = { .x = 0, .y = 0, .val = -1 };
  for(int i = 0; i < s->ne4; ++i) {
    if(s->table[i] == 0) {
      buf.y = i / s->ne2;
      buf.x = i % s->ne2;
      break;
    }
  }
  display(s, buf.y, buf.x);
keyboard:;
  while((ch = getch()) != 27) {
    switch(ch) {
      case KEY_LEFT: case 'h':
        if(0 < buf.x)
          update_cell(s, buf),
          --buf.x,
          buf.val = -1;
      break;
      case KEY_RIGHT: case 'l':
        if(s->ne2 - 1 != buf.x)
          update_cell(s, buf),
          ++buf.x,
          buf.val = -1;
      break;
      case KEY_UP: case 'k':
        if(0 < buf.y)
          update_cell(s, buf),
          --buf.y,
          buf.val = -1;
      break;
      case KEY_DOWN: case 'j':
        if(s->ne2 - 1 != buf.y)
          update_cell(s, buf),
          ++buf.y,
          buf.val = -1;
      break;
      case ' ':
        compile_sudoku(s);
      break;
      case '\t':
        solve_position(s, buf);
      break;
      default:
        if(isdigit(ch)) {
          if(buf.val == -1) {
            buf.val = 0;
          }
          int d = ch - '0';
          buf.val = 10 * buf.val + d;
          if(buf.val > s->ne2)
            buf.val = d;
          update_cell(s, buf);
        } else continue;
      break;
    }
    display(s, buf.y, buf.x);
  }
  endwin();
  return EXIT_SUCCESS;
}

main(int argc, char *argv[]) {
making_sudoku:;
  ui_sudoku_t s;
  scanf(" %d", &s.n);
  s.ne2 = s.n*s.n;
  s.ne4 = s.ne2 * s.ne2;
  s.table = alloca(sizeof(uint8_t) * s.ne4);
  s.res = INCOMPLETE;
  s.compiled = false;
setting_board:;
  for(int i = 0; i < s.ne4; ++i) {
    int sc = scanf("%hhu", &s.table[i]);
    assert(sc == 1);
  }
  int uiret = start_ui(&s);
  if(uiret != EXIT_SUCCESS)
    return uiret;
print_composed_sudoku:;
  puts("");
  printf("%d\n", s.n);
  for(int i = 0; i < s.ne2; ++i) {
    for(int j = 0; j < s.ne2; ++j) {
      uint8_t t = s.table[s.ne2 * i + j];
      int len = chrlen(t);
      for(int k = 0; k < 3 - len; ++k)
        putchar(' ');
      printf("%hhu", t);
    }
    putchar('\n');
  }
}
