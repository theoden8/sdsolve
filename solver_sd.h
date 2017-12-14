#ifndef SUDOKU_H_JTU2YWOL
#define SUDOKU_H_JTU2YWOL

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint8_t val_t;
typedef uint32_t sz_t;

typedef enum {
  INVALID,
  MULTIPLE,
  INCOMPLETE,
  COMPLETE
} sudoku_res_t;

typedef struct _sudoku_t {
  val_t *table;
  sz_t n, n_e2, n_e4;
  sz_t size;
  sudoku_res_t res;
  sz_t no_virts;
  int depth;
} sudoku_t;

#define CELL_SIZE(s) ((s->n_e2 + (8*sizeof(val_t))) / (8*sizeof(val_t)))

sudoku_t *alloc_sudoku(sz_t n);
int dump_cell(const sudoku_t *s, sz_t pos);
void dump_sudoku(const sudoku_t *s);
sudoku_t *cpy_sudoku(const sudoku_t *s);
void free_sudoku(sudoku_t **s);
sz_t make_pos_xy(const sudoku_t *s, sz_t x, sz_t y);
sz_t make_pos_yx(const sudoku_t *s, sz_t y, sz_t x);

sz_t get_pos_x(const sudoku_t *s, sz_t pos);
sz_t get_pos_y(const sudoku_t *s, sz_t pos);
sz_t get_pos_box(const sudoku_t *s, sz_t pos);
sz_t get_pos_byte_idx(const sudoku_t *s, sz_t pos);
sz_t get_pos_byte_idx_d(const sudoku_t *s, sz_t pos, sz_t x);

#define BIT_OFFSET_X(f) (1 << (((f) - 1) % (CHAR_BIT * sizeof(val_t))))

bool is_virt_sudoku(const sudoku_t *s, sz_t pos);
bool is_virt_sudoku_dd(const sudoku_t *s, sz_t pos, val_t t);
void set_sudoku_virt(sudoku_t *s, sz_t pos);
void set_sudoku_virt_d(sudoku_t *s, sz_t pos, val_t t);
void set_sudoku_virt_all(sudoku_t *s, sz_t pos);
void unset_sudoku_virt(sudoku_t *s, sz_t pos);
void unset_sudoku_virt_d(sudoku_t *s, sz_t pos, val_t t);
void unset_sudoku_virt_all(sudoku_t *s, sz_t pos);
val_t find_unique_virt_val(const sudoku_t *s, sz_t pos);
val_t get_sudoku_rightmost_virt(const sudoku_t *s, sz_t pos);
val_t count_sudoku_virts(const sudoku_t *s, sz_t pos);
val_t get_sudoku_val(const sudoku_t *s, sz_t pos);
bool is_defined_val(const sudoku_t *s, sz_t pos);
void set_last_byte_val(sudoku_t *s, sz_t pos, val_t x);
void set_sudoku_val_d(sudoku_t *s, sz_t pos, val_t t);

#endif
