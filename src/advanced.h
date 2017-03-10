#ifndef SUDOKU_H_USO45YBJ
#define SUDOKU_H_USO45YBJ

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t sz_t;
typedef uint8_t val_t;

#define RIDX(C,R) ((C)*s->ne2 + (R))
#define CIDX(R, C) ((R) << 2 | (C))

typedef struct _sd_t {
  sz_t n, ne2, ne4;
  val_t *table;
  sz_t w, h, wy;
  sz_t *r, *c;
} sd_t;

typedef enum { ROWCOL, BOXNUM, ROWNUM, COLNUM, NO_CONSTR } CONSTRAINTS;

sd_t *make_sd(sz_t n, val_t *table);
void free_sd(sd_t *s);
typedef enum { INVALID, COMPLETE, MULTIPLE } RESULT;

typedef struct _cov_t {
  val_t *row, *col;
  sz_t *colfail;
} cov_t;

typedef struct _sol_t {
  sz_t *row, *col;
} sol_t;

typedef enum { FORWARD, BACKTRACK } ACTION;

#define MINUNDEF (s->ne2 + 1)
typedef struct _min_t {
  val_t min;
  sz_t fail_rate;
  sz_t min_col;
} min_t;

extern sz_t UNDEF_SIZE;

static inline void sd_forward(const sd_t *s, cov_t *covered, sz_t r, sz_t c, min_t *m);
static inline void sd_revert(const sd_t *s, cov_t *covered, sz_t r, sz_t c);
static inline min_t sd_update(const sd_t *s, cov_t *covered, sz_t r, ACTION flag);
RESULT solve_sd(sd_t *s);

#endif
