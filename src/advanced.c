#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <alloca.h>
#include <stdbool.h>
#include <math.h>

#include "advanced.h"

// the implementation idea is taken (mainly) from
//   https://raw.githubusercontent.com/attractivechaos/plb/master/sudoku/sudoku_v1.c
// fully written from scratch

const sz_t UNDEF_SIZE = -1;

sd_t *make_sd(sz_t n, val_t *table) {
attributes:;
  sd_t *s=malloc(sizeof(sd_t));assert(s != NULL);
  s->n = n;
  s->ne2 = n*n;
  s->ne4 = s->ne2 * s->ne2;
  s->table=malloc(sizeof(val_t)*s->ne4),assert(s->table!=NULL);
  memcpy(s->table, table, sizeof(val_t) * s->ne4);
// constraint table
  s->w = s->ne4 * NO_CONSTR;
  s->h = s->ne4 * s->ne2;
col:;
  s->c=malloc(sizeof(sz_t)*s->h*NO_CONSTR),assert(s->c!=NULL);
  for(sz_t r = 0; r < s->ne2; ++r) {
    for(sz_t c = 0; c < s->ne2; ++c) {
      for(sz_t v = 0; v < s->ne2; ++v) {
        sz_t *it = &C_CNSTR(r * s->ne4 + c * s->ne2 + v, 0);
        it[ROWCOL] = s->ne4 * ROWCOL + s->ne2 * r + c,
        it[BOXNUM] = s->ne4 * BOXNUM + (r / s->n * s->n + c / s->n) * s->ne2 + v,
        it[ROWNUM] = s->ne4 * ROWNUM + s->ne2 * r + v,
        it[COLNUM] = s->ne4 * COLNUM + s->ne2 * c + v;
      }
    }
  }
row:;
  val_t mem[s->w];
  for(sz_t i=0;i<s->w;++i)mem[i]=0;
  s->r=malloc(sizeof(sz_t)*s->w*s->ne2),assert(s->r!=NULL);
  for(sz_t r = 0; r < s->h; ++r) {
    for(sz_t c = 0; c < NO_CONSTR; ++c) {
      sz_t i = C_CNSTR(r, c);
      assert(0 <= i && i < s->w);
      R_SLNS(i,mem[i]) = r, ++mem[i];
    }
  }
// solver
cov:;
  size_t
    cov_header = sizeof(cov_t),
    cov_row = sizeof(val_t)*s->h,
    cov_col = sizeof(val_t)*s->w,
    cov_colfail = sizeof(sz_t) * s->w,
    cov_colchoice = sizeof(sz_t) * s->w;
  s->cov=malloc(cov_header+cov_row+cov_col+cov_colfail+cov_colchoice),assert(s->cov != NULL);
  void *first = (void *)s->cov;
  s->cov->row = (first+=cov_header);
  s->cov->col = (first+=cov_row);
  s->cov->colfail = (first+=cov_col);
  s->cov->colchoice = (first+=cov_colfail);
soln:;
  size_t
    soln_header = sizeof(sol_t),
    soln_row = sizeof(sz_t) * s->ne4,
    soln_col = sizeof(sz_t) * s->ne4;
  s->soln=malloc(soln_header+soln_row+soln_col),assert(s->soln != NULL);
  first = (void *)s->soln;
  s->soln->row = (first+=soln_header);
  s->soln->col = (first+=soln_row);
buf:;
  s->buf=malloc(sizeof(val_t)*s->ne4),assert(s->buf != NULL);
ret:;
  return s;
}

static void dump_constraint_table(const sd_t *s) {
  for(sz_t i = 0; i < s->h; ++i) {
    printf("[%d|%d|%d] ", i / s->ne4, i / s->ne2 % s->ne2, i % s->ne2);
    for(sz_t k = 0; k < s->w; ++k) {
      if(!(k % s->ne4)) {
        printf("  ");
      }
      bool x = (k == C_CNSTR(i, k / s->ne4));
      printf("%d ", x);
    }
    putchar('\n');
  }
}

static void dump_ctable(const sd_t *s) {
  puts("dump ctable");
  sz_t i = 0;
  for(sz_t r = 0; r < s->ne2; ++r) {
    for(sz_t c = 0; c < s->ne2; ++c) {
      for(sz_t v = 0; v < s->ne2; ++v) {
        printf("[%d|%d|%d] = ", r, c, v);
        for(sz_t cc = 0; cc < NO_CONSTR; ++cc) {
          int l = printf("%u", C_CNSTR(i, cc));
          for(int j=0;j<2+s->n-l;++j)putchar(' ');
        }
        putchar('\n'),++i;
      }
    }
  }
  fflush(stdout);
}

static void dump_rtable(const sd_t *s) {
  puts("dump rtable");
  sz_t i = 0;
  for(sz_t cc = 0; cc < NO_CONSTR; ++cc) {
    for(sz_t r = 0; r < s->ne2; ++r) {
      for(sz_t c = 0; c < s->ne2; ++c) {
        printf("%u: [%u|%u] = ", cc, r, c);
        for(sz_t v = 0; v < s->ne2; ++v) {
          int l = printf("%u", R_SLNS(i, v));
          for(int j=0;j<2+s->n-l;++j)putchar(' ');
        }
        putchar('\n'),++i;
      }
    }
  }
  fflush(stdout);
}

void free_sd(sd_t *s) {
  free(s->table);
  free(s->r);
  free(s->c);
  if(s->cov != NULL)
    free(s->cov);
  if(s->soln != NULL)
    free(s->soln);
  if(s->buf != NULL)
    free(s->buf);
  free(s);
}

static void dump_covererd(sd_t *s, sz_t len1, sz_t len2) {
  puts("covered");
  printf("row: "); for(sz_t i=0;i<len1;++i)printf("%hhd ", s->cov->row[i]);putchar('\n');
  printf("col: "); for(sz_t i=0;i<len2;++i)printf("%hhd ", s->cov->col[i]);putchar('\n');
}

static void dump_choice(sd_t *s, sz_t len1, sz_t len2) {
  puts("choice");
  printf("row: "); for(sz_t i=0;i<len1;++i)printf("%d ", s->soln->row[i]);putchar('\n');
  printf("col: "); for(sz_t i=0;i<len2;++i)printf("%d ", s->soln->col[i]);putchar('\n');
}

static inline min_t default_min(const sd_t *s) {
  return (min_t){
    .min = MINUNDEF,
    .fail_rate = 0,
    .choice_rate = 0,
    .min_col = 0,
  };
}

static inline min_t sd_update(const sd_t *s, sz_t r, ACTION flag) {
  min_t m = default_min(s);
  const static val_t LBIT = 1 << (8 * sizeof(val_t) - 1);
  for(sz_t c = 0; c < NO_CONSTR; ++c)s->cov->col[C_CNSTR(r, c)] ^= LBIT;
  for(sz_t c = 0; c < NO_CONSTR; ++c)
    if(flag==FORWARD)sd_forward(s,r,c,&m);else
      sd_backtrack(s,r,c);
  return m;
}

static inline void sd_forward(const sd_t *s, sz_t r, sz_t c, min_t *m) {
  assert(c < NO_CONSTR);
  const sz_t clm = C_CNSTR(r, c);
  assert(clm < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = R_SLNS(clm, ir);
    assert(rr < s->h);
    if(s->cov->row[rr]++ != 0)continue;
    for(sz_t ic = 0; ic < NO_CONSTR; ++ic) {
      sz_t cc = C_CNSTR(rr, ic);
      assert(clm < s->w);
      if(--s->cov->col[cc] < m->min)
        m->min=s->cov->col[cc],m->fail_rate=s->cov->colfail[cc],
        m->choice_rate=s->cov->colchoice[cc],m->min_col=cc;
    }
  }
}

static inline void sd_backtrack(const sd_t *s, sz_t r, sz_t c) {
  assert(c < NO_CONSTR);
  sz_t clm = C_CNSTR(r, c);
  assert(clm < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = R_SLNS(clm, ir);
    assert(rr < s->h);
    --s->cov->row[rr];
    assert(0 <= s->cov->row[rr] && s->cov->row[rr] <= NO_CONSTR);
    if(s->cov->row[rr] != ROWCOL)continue;
    sz_t *it = &C_CNSTR(rr, 0);
    ++s->cov->col[it[ROWCOL]], ++s->cov->col[it[BOXNUM]],
    ++s->cov->col[it[ROWNUM]], ++s->cov->col[it[COLNUM]];
  }
}

static void dump_sd(const sd_t *s, int i) {
  static counter = 0;
  if(!s){counter=0;return;}
  printf("[%d] dump sudoku\n", counter);
  val_t dump[s->ne4];
  for(int i=0;i<s->ne4;++i)dump[i]=0;
  memcpy(dump, s->table, sizeof(val_t) * s->ne4);
  for(int j = 0; j < i; ++j) {
    int r = R_SLNS(s->soln->col[j], s->soln->row[j]);
    dump[r / s->ne2] = r % s->ne2 + 1;
  }
  for(sz_t i = 0; i < s->ne2; ++i) {
    for(sz_t j = 0; j < s->ne2; ++j) {
      printf(" %d", dump[i * s->ne2 + j]);
    }
    putchar('\n');
  }
  ++counter;
  fflush(stdout);
}

static inline bool check_sd(sd_t *s) {
  val_t *check = alloca(s->ne2*3);
  assert(check != NULL);
  for(sz_t i = 0; i < s->ne2; ++i) {
    memset(check, 0x00, s->ne2 * 3 * sizeof(val_t));
    for(sz_t j = 0; j < s->ne2; ++j) {
      sz_t pos[3] = { i*s->ne2 + j, j*s->ne2 + i, (i/s->n*s->n + j/s->n)*s->ne2 + (i%s->n)*s->n + j%s->n };
      for(int k = 0; k < 3; ++k) {
        sz_t p = pos[k];
        val_t t = s->table[p];
        if(!t)continue;
        if(check[k*s->ne2 + t-1]) {
          return false;
        }
        check[k*s->ne2 + t-1] = true;
      }
    }
  }
  return true;
}

static inline void sd_reset(sd_t *s) {
  s->no_hints=0;
  for(sz_t i=0;i<s->h;++i)s->cov->row[i]=ROWCOL;
  for(sz_t i=0;i<s->w;++i)s->cov->col[i]=s->ne2;
  for(sz_t i=0;i<s->w;++i)s->cov->colfail[i]=0;
  for(sz_t i=0;i<s->w;++i)s->cov->colchoice[i]=0;
}

static inline void sd_forward_knowns(sd_t *s) {
  sd_reset(s);
  for(sz_t i = 0; i < s->ne4; ++i) {
    val_t t = s->table[i];
    if(t) {
      sd_update(s, i * s->ne2 + t - 1, FORWARD);
      ++s->no_hints;
    }
    s->soln->row[i] = s->soln->col[i] = UNDEF_SIZE, s->buf[i] = t;
  }
}

RESULT solve_sd(sd_t *s) {
  RESULT res = INVALID;
  if(!check_sd(s))return res=INVALID;
presetup:;
  sd_forward_knowns(s);
  ACTION action = FORWARD;
  min_t m = default_min(s);
iterate_unknowns:;
  int_fast32_t i = 0;
  const int_fast32_t no_vars = s->ne4 - s->no_hints;
  while(1) {
    while(i >= 0 && i < no_vars) {
      /* dump_sd(s, i); */
      if(action == FORWARD) {
        s->soln->col[i] = m.min_col;
        if(m.min > 1) {
          for(sz_t c = 0; c < s->w; ++c) {
            if(s->cov->col[c] < m.min || (s->cov->col[c] == m.min && s->cov->colfail[c] > m.fail_rate)) {
              m.min=s->cov->col[c],
              m.fail_rate=s->cov->colfail[c],
              m.min_col=c,
              s->soln->col[i]=c;
              if(m.min < 2)break;
            }
          }
        } else if(m.min == MINUNDEF) {
          s->cov->colfail[m.min_col] = s->cov->colchoice[m.min_col],
          action = BACKTRACK,
          s->soln->row[i] = UNDEF_SIZE,
          --i;
        }
      }
      assert(i >= -1);
      const int_fast32_t ii = (i == -1) ? 0 : i;
      const sz_t cc = s->soln->col[ii], cr = s->soln->row[ii];
      assert(cc != UNDEF_SIZE),assert(cc < s->h),assert(cr == UNDEF_SIZE || cr < s->w);
      if(action == BACKTRACK && cr != UNDEF_SIZE)
        sd_update(s, R_SLNS(cc, cr), BACKTRACK),
        s->cov->colfail[cc] = s->cov->colchoice[cc];
      val_t ir = (cr == UNDEF_SIZE) ? 0 : cr + 1;
      while(ir < s->ne2) {
        if(s->cov->row[R_SLNS(cc, ir)] == 0)break;
        ++ir;
      }
      if(ir < s->ne2) {
        const sz_t diff=s->ne2-m.min;
        s->cov->colchoice[cc] += diff*diff*(no_vars-i) / s->w + 1;
        /* ++s->cov->colchoice[cc]; */
        action = FORWARD,
        m = sd_update(s, R_SLNS(cc, ir), FORWARD),
        s->soln->row[ii] = ir,
        ++i;
      } else {
        s->cov->colfail[m.min_col] = s->cov->colchoice[m.min_col],
        action = BACKTRACK,
        s->soln->row[ii] = UNDEF_SIZE,
        --i;
      }
    }
    if(i < 0)break;
  get_sdtable:;
    for(sz_t j = 0; j < i; ++j) {
      sz_t r = R_SLNS(s->soln->col[j], s->soln->row[j]);
      assert(r < s->h);
      s->buf[r / s->ne2] = r % s->ne2 + 1;
    }
  change_res:;
    switch(res) {
      case INVALID:
        res = COMPLETE;
        memcpy(s->table, s->buf, sizeof(val_t) * s->ne4);
      break;
      case COMPLETE:
        res = MULTIPLE;
        goto endsolve;
      break;
      case MULTIPLE:
        assert(res != MULTIPLE);
      break;
    }
    --i,action=BACKTRACK;
  }
endsolve:
  return res;
}
