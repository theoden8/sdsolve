#ifndef SUDOKU_H_USO45YBJ
#define SUDOKU_H_USO45YBJ

#include <stdint.h>
#include <stdbool.h>

// (CONSTRx(RxC))xN
#define RIDX(C,R) ((C)*s->ne2 + (R))
#define R_SLNS(C, R) s->r[RIDX(C, R)]

// (RxCxN)xCONSTR
#define CIDX(R, C) (((R) << 2) | (C))
#define C_CNSTR(R, C) s->c[CIDX(R, C)]

typedef uint_fast8_t val_t;
typedef int_fast32_t sz_t;

struct _cov_t;
struct _sol_t;

typedef enum { FORWARD, BACKTRACK } ACTION;

typedef struct _sd_t {
  // general
  sz_t n, ne2, ne3, ne4;
  val_t *table;
  // constraint table
  sz_t w, h, wy;
  sz_t *r, *c;
  // solver
  sz_t no_hints, no_vars;
  int_fast32_t i;
  ACTION action;
  struct _cov_t *cov;
  struct _sol_t *soln;
  val_t *buf;
} sd_t;

typedef enum { ROWCOL, BOXNUM, ROWNUM, COLNUM, NO_CONSTR } CONSTRAINTS;

sd_t *make_sd(sz_t n, val_t *table);
void free_sd(sd_t *s);
typedef enum { INVALID, COMPLETE, MULTIPLE } RESULT;

typedef struct _cov_t {
  val_t *row, *col;
  sz_t *colfail, *colchoice;
} cov_t;

typedef struct _sol_t {
  sz_t *row, *col;
} sol_t;

#define MINUNDEF (s->ne2 + 1)
typedef struct _min_t {
  val_t min;
  sz_t min_col;
  sz_t fail_rate;
  sz_t choice_rate;
} min_t;

extern const sz_t UNDEF_SIZE;

static inline void sd_update(const sd_t *s, sz_t r, ACTION flag);
static inline void sd_update_min(const sd_t *s, sz_t r, ACTION flag, min_t *m);
static inline void sd_forward(const sd_t *s, sz_t r, sz_t c);
static inline void sd_forward_min(const sd_t *s, sz_t r, sz_t c, min_t *m);
static inline void sd_backtrack(const sd_t *s, sz_t r, sz_t c);
void sd_setboard(sd_t *s, val_t *table);
RESULT solve_sd(sd_t *s);

#endif
