#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "solver_sd.h"
#include "solver_io.h"

// allocate sudoku board on heap
sudoku_t *alloc_sudoku(const sz_t n) {
  sudoku_t *s = malloc(sizeof(sudoku_t));
  assert(s != NULL);
  s->n = n;
  s->n_e2 = n*n;
  s->n_e4 = s->n_e2*s->n_e2;
  s->size = s->n_e4 * CELL_SIZE(s);
  s->table = malloc(s->size);
  memset(s->table, 0x00, s->size);
  assert(s->table != NULL);
  s->res = INCOMPLETE;
  s->no_virts = 0;
  s->depth = 0;
  return s;
}

// this function is only used for debugging purposes
// prints a given cell in hex
int dump_cell(const sudoku_t *s, sz_t pos) {
  int len = 0;
  for(int c = 0; c < CELL_SIZE(s); ++c) {
    len += printf("%hhX", s->table[pos * CELL_SIZE(s) + c]);
  }
  return len;
}

// this function is only used for debugging purposes
// dump the full information about a sudoku_t object
void dump_sudoku(const sudoku_t *s) {
  printf("n=%u\nnn=%u\ncellsize=%lu\nsize=%u\nno_virts=%u\n",
         s->n, s->n_e2, CELL_SIZE(s),
         s->size, s->no_virts);
  printf("\n");
  for(sz_t i = 0; i < s->n_e2; ++i) {
    for(sz_t j = 0; j < s->n_e2; ++j) {
      int len = dump_cell(s, make_pos_yx(s, i, j));
      for(int i = len; i < 5; ++i)putchar(' ');
      if(j % s->n == s->n-1 && j != s->n_e2-1)printf("| ");
      printf(" ");
    }
    printf("\n");
    if(i % s->n == s->n-1 && i != s->n_e2-1) {
      for(int i=0;i<s->n_e2*6+s->n*2-3;++i)putchar('-');
      printf("\n");
    }
  }
  printf("result: %s\n", sudoku_res_to_string(s->res));
  printf("\n");
  fflush(stdout);
}

// allocate on heap and copy
sudoku_t *cpy_sudoku(const sudoku_t *s) {
  sudoku_t *ret = alloc_sudoku(s->n);
  memcpy(ret->table, s->table, s->size);
  ret->n = s->n;
  ret->n_e2 = s->n_e2;
  ret->n_e4 = s->n_e4;
  ret->size = s->size;
  ret->res = s->res;
  ret->no_virts = s->no_virts;
  ret->depth = s->depth;
  return ret;
}

// free the memory and set the pointer to NULL
void free_sudoku(sudoku_t **s) {
  free((*s)->table);
  free(*s);
  *s = NULL;
}

// make the position index out of given x and y coords
sz_t make_pos_xy(const sudoku_t *s, sz_t x, sz_t y) {
  return y * s->n_e2 + x;
}

// make the position index out of given y and x coords
sz_t make_pos_yx(const sudoku_t *s, sz_t y, sz_t x) {
  sz_t pos = make_pos_xy(s, x, y);
  assert(make_pos_xy(s, get_pos_x(s, pos), get_pos_y(s, pos)) == pos);
  return make_pos_xy(s, x, y);
}

// get x coord out of given position index
sz_t get_pos_x(const sudoku_t *s, sz_t pos) {
  return pos % s->n_e2;
}

// get y coord out of given position index
sz_t get_pos_y(const sudoku_t *s, sz_t pos) {
  return pos / s->n_e2;
}

// get the box in which given position index belongs to
sz_t get_pos_box(const sudoku_t *s, sz_t pos) {
  return get_pos_y(s, pos) / s->n * s->n + get_pos_x(s, pos) / s->n;
}

// get the byte number in the array from the position index.
// e.g. for n=2 sudoku its 0, 1, 2, 3, 4, ..
// for n=3 its 0, 2, 4, 6, 8, ..
// for n=4 its 0, 3, 6, 9, 12, ..
// for n=9 its 0, 11, 22, 33, ...
// essentially, pos * ceil((n**2 + 1) / 8)
sz_t get_pos_byte_idx(const sudoku_t *s, sz_t pos) {
  return pos * CELL_SIZE(s);
}

// get the offset of nth byte for given pos index
sz_t get_pos_byte_idx_d(const sudoku_t *s, sz_t pos, sz_t x) {
  return (get_pos_byte_idx(s, pos + 1)  - ((x - 1) / (8 * sizeof(char))) - 1);
}

// whether the first bit of the cell is set or not (i.e. 0 means it has a value,
// 1 means it only has possible values)
bool is_virt_sudoku(const sudoku_t *s, sz_t pos) {
  return s->table[get_pos_byte_idx(s, pos)] & BIT_OFFSET_X(8 * sizeof(char));
}

// tells whether the value "t" is marked possiblef for this cell
bool is_virt_sudoku_dd(const sudoku_t *s, sz_t pos, val_t t) {
  if(!is_virt_sudoku(s, pos))
    return false;
  return s->table[get_pos_byte_idx_d(s, pos, t)] & BIT_OFFSET_X(t);
}

// set the first bit
void set_sudoku_virt(sudoku_t *s, sz_t pos) {
  assert(!is_virt_sudoku(s, pos));
  ++s->no_virts;
  s->table[get_pos_byte_idx(s, pos)] |= BIT_OFFSET_X(8 * sizeof(char));
  assert(is_virt_sudoku(s, pos));
}

// set the t-th bit
void set_sudoku_virt_d(sudoku_t *s, sz_t pos, val_t t) {
  assert(t != 0);
  assert(is_virt_sudoku(s, pos));
  s->table[get_pos_byte_idx_d(s, pos, t)] |= BIT_OFFSET_X(t);
  assert(is_virt_sudoku_dd(s, pos, t));
}

// set as virtual and set all possibility-related bits
void set_sudoku_virt_all(sudoku_t *s, sz_t pos) {
  assert(!is_virt_sudoku(s, pos));
  set_sudoku_virt(s, pos);
  for(sz_t i = s->n_e2 / (8*sizeof(char)) * (8*sizeof(char)); i < s->n_e2; ++i) {
    set_sudoku_virt_d(s, pos, i + 1);
  }
  memset(s->table + get_pos_byte_idx(s, pos) + 1, 0xff, CELL_SIZE(s) - 1);
  for(sz_t i = 0; i < s->n_e2; ++i) {
    assert(is_virt_sudoku_dd(s, pos, i + 1));
  }
  assert(is_virt_sudoku(s, pos));
}

// unset the 1st bit
void unset_sudoku_virt(sudoku_t *s, sz_t pos) {
  assert(is_virt_sudoku(s, pos));
  --s->no_virts;
  s->table[get_pos_byte_idx(s, pos)] &= ~BIT_OFFSET_X(8 * sizeof(char));
  assert(!is_virt_sudoku(s, pos));
}

// unset the possibility bit for a value t
void unset_sudoku_virt_d(sudoku_t *s, sz_t pos, val_t t) {
  assert(t != 0);
  assert(is_virt_sudoku(s, pos));
  s->table[get_pos_byte_idx_d(s, pos, t)] &= ~BIT_OFFSET_X(t);
  assert(!is_virt_sudoku_dd(s, pos, t));
}

// unset all possibilities and the first bit
void unset_sudoku_virt_all(sudoku_t *s, sz_t pos) {
  assert(is_virt_sudoku(s, pos));
  for(sz_t i = s->n_e2 / (8*sizeof(char)) * (8*sizeof(char)); i < s->n_e2; ++i) {
    unset_sudoku_virt_d(s, pos, i + 1);
  }
  memset(s->table + get_pos_byte_idx(s, pos) + 1, 0x00, CELL_SIZE(s) - 1);
  for(sz_t i = 0; i < s->n_e2; ++i) {
    assert(!is_virt_sudoku_dd(s, pos, i + 1));
  }
  unset_sudoku_virt(s, pos);
}

// if there is only one possibility left, return what should the cell be set to then
val_t find_unique_virt_val(const sudoku_t *s, sz_t pos) {
  val_t val = 0;
  assert(0 <= val && val <= s->n_e2);
  bool
    no_powers = false,
    all_zeros = true;
  for(sz_t i = 0; i < CELL_SIZE(s); ++i) {
    unsigned char mask = s->table[get_pos_byte_idx(s, pos) + i];
    if(i == 0)
      mask &= 0x7F; // all but the first bit
    if(mask) {
      all_zeros = false;
      if(!no_powers)
        no_powers = true;
      else
        return 0;
    }
    if(mask && !(mask & (mask - 1))) {
      val = (CELL_SIZE(s) - 1 - i) * 8 + 1;
      while(mask >>= 1)
        ++val;
    } else if(mask && no_powers) {
      return 0;
    }
  }
  if(all_zeros)
    return 255; // aka -1
  assert(0 <= val && val <= s->n_e2);
  return val;
}

// get rightmost set possibility bit
val_t get_sudoku_rightmost_virt(const sudoku_t *s, sz_t pos) {
  val_t right = 0;
  for(sz_t i = 0; i < CELL_SIZE(s); ++i) {
    unsigned char mask = s->table[get_pos_byte_idx(s, pos + 1) - 1 - i];
    if(i == CELL_SIZE(s) - 1) {
      mask &= 0x7f;
    }
    if(!mask)continue;
    mask &= ~(mask - 1);
    right = 8 * sizeof(char) * i + 1;
    while(mask >>= 1)++right;
    if(right)
      return right;
  }
  return right;
}

static unsigned char count_bits(unsigned char x) {
  x = x - ((x >> 1) & 0x55);
  x = (x & 0x33) + ((x >> 2) & 0x33);
  x = (x + (x >> 4)) & 0x0F;
  return (x * 0x01);
}

// counts the number of possibility bits
val_t count_sudoku_virts(const sudoku_t *s, sz_t pos) {
  if(!is_virt_sudoku(s, pos)) {
    return 0;
  }
  val_t q = 0;
  for(sz_t i = 0; i < CELL_SIZE(s); ++i) {
    unsigned char mask = s->table[get_pos_byte_idx(s, pos) + i];
    if(!i)mask&=0x7f;
    q += count_bits(mask);
  }
  return q;
}

// if virtual, 0, else what the value is
val_t get_sudoku_val(const sudoku_t *s, sz_t pos) {
  if(is_virt_sudoku(s, pos))
    return 0;
  assert(!is_virt_sudoku(s, pos));
  val_t t = s->table[get_pos_byte_idx(s, pos + 1) - 1];
  assert(t != 0);
  return t;
}

// same as !is_virt_sudoku, but more high-level
bool is_defined_val(const sudoku_t *s, sz_t pos) {
  return get_sudoku_val(s, pos);
}

// used by the set_sudoku_val_d. separated for possible use of counters and asserts
void set_last_byte_val(sudoku_t *s, sz_t pos, val_t x) {
  s->table[get_pos_byte_idx(s, pos) + CELL_SIZE(s) - 1] = x;
}

// initialize (reinitialize) the cell: 0 to set all possibilities, anything else
// to set the actual value
void set_sudoku_val_d(sudoku_t *s, sz_t pos, val_t x) {
  if(x == 0) {
    set_sudoku_virt_all(s, pos);
    return;
  }
  assert(x <= s->n_e2);
  if(is_virt_sudoku(s, pos)) {
    unset_sudoku_virt_all(s, pos);
  }
  set_last_byte_val(s, pos, x);
  assert(!is_virt_sudoku(s, pos));
  assert(is_defined_val(s, pos));
}
