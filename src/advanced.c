#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <alloca.h>
#include <stdbool.h>

#include "advanced.h"

// the implementation idea is taken (mainly) from
//   https://raw.githubusercontent.com/attractivechaos/plb/master/sudoku/sudoku_v1.c
// fully written from scratch

sz_t UNDEF_SIZE = -1;

sd_t *make_sd(sz_t n, val_t *table) {
attributes:;
  sd_t *s = malloc(sizeof(sd_t)); assert(s != NULL);
  s->n = n;
  s->ne2 = n*n;
  s->ne4 = s->ne2 * s->ne2;
  s->table=malloc(sizeof(val_t)*s->ne4),assert(s->table!=NULL);
  memcpy(s->table, table, sizeof(val_t) * s->ne4);
  s->w = s->ne4 * NO_CONSTR;
  s->h = s->ne4 * s->ne2;
col:;
  s->c=malloc(sizeof(sz_t)*s->h*NO_CONSTR),assert(s->c!=NULL);
  for(sz_t r = 0; r < s->ne2; ++r) {
    for(sz_t c = 0; c < s->ne2; ++c) {
      for(sz_t v = 0; v < s->ne2; ++v) {
        sz_t *it = &CVAL(r * s->ne4 + c * s->ne2 + v, 0);
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
  s->r = malloc(sizeof(sz_t)*s->w*s->ne2),assert(s->r!=NULL);
  for(sz_t r = 0; r < s->h; ++r) {
    for(sz_t c = 0; c < NO_CONSTR; ++c) {
      sz_t i = CVAL(r, c);
      assert(0 <= i && i < s->w);
      RVAL(i,mem[i]) = r, ++mem[i];
    }
  }
  return s;
}

static void dump_constraint_table(const sd_t *s) {
  for(sz_t i = 0; i < s->h; ++i) {
    printf("[%d|%d|%d] ", i / s->ne4, i / s->ne2 % s->ne2, i % s->ne2);
    for(sz_t k = 0; k < s->w; ++k) {
      if(!(k % s->ne4)) {
        printf("  ");
      }
      bool x = (k == CVAL(i, k / s->ne4));
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
          int l = printf("%u", CVAL(i, cc));
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
          int l = printf("%u", RVAL(i, v));
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
  free(s);
}

static void dump_covererd(const cov_t *pair, sz_t len1, sz_t len2) {
  puts("covered");
  printf("row: "); for(sz_t i=0;i<len1;++i)printf("%hhd ", pair->row[i]);putchar('\n');
  printf("col: "); for(sz_t i=0;i<len2;++i)printf("%hhd ", pair->col[i]);putchar('\n');
}

static void dump_choice(const sol_t *pair, sz_t len1, sz_t len2) {
  puts("choice");
  printf("row: "); for(sz_t i=0;i<len1;++i)printf("%d ", pair->row[i]);putchar('\n');
  printf("col: "); for(sz_t i=0;i<len2;++i)printf("%d ", pair->col[i]);putchar('\n');
}

static inline min_t sd_update(const sd_t *s, cov_t *cov, sz_t r, ACTION flag) {
  min_t m = {
    .min = MINUNDEF,
    .fail_rate = UNDEF_SIZE,
    .min_col = 0,
  };
  const static val_t LBIT = 1 << (8 * sizeof(val_t) - 1);
  for(sz_t c = 0; c < NO_CONSTR; ++c)cov->col[CVAL(r, c)] ^= LBIT;
  for(sz_t c = 0; c < NO_CONSTR; ++c)
    if(flag==FORWARD)sd_forward(s,cov,r,c,&m);else
      sd_revert(s,cov,r,c);
  return m;
}

static inline void sd_forward(const sd_t *s, cov_t *cov, sz_t r, sz_t c, min_t *m) {
  assert(c < NO_CONSTR);
  const sz_t cidx = CVAL(r, c);
  assert(cidx < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = RVAL(cidx, ir);
    assert(rr < s->h);
    if(cov->row[rr]++ != 0)continue;
    for(sz_t ic = 0; ic < NO_CONSTR; ++ic) {
      sz_t cc = CVAL(rr, ic);
      assert(cidx < s->w);
      --cov->col[cc];
      if(cov->col[cc] < m->min)
        m->min=cov->col[cc],m->fail_rate=cov->colfail[cc],m->min_col=cc;
    }
  }
}

static inline void sd_revert(const sd_t *s, cov_t *cov, sz_t r, sz_t c) {
  assert(c < NO_CONSTR);
  sz_t cidx = CVAL(r, c);
  assert(cidx < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = RVAL(cidx, ir);
    assert(rr < s->h);
    --cov->row[rr];
    assert(0 <= cov->row[rr] && cov->row[rr] <= NO_CONSTR);
    if(cov->row[rr] != ROWCOL)continue;
    sz_t *it = &CVAL(rr, 0);
    ++cov->col[it[ROWCOL]], ++cov->col[it[BOXNUM]],
    ++cov->col[it[ROWNUM]], ++cov->col[it[COLNUM]];
  }
}

static void dump_sd(sd_t *s, sol_t soln, int i) {
  static counter = 0;
  printf("[%d] dump sudoku\n", counter);
  val_t dump[s->ne4];
  for(int i=0;i<s->ne4;++i)dump[i]=0;
  /* memcpy(dump, s->table, sizeof(val_t) * s->ne4); */
  for(int j = 0; j < i; ++j) {
    int r = RVAL(soln.col[j], soln.row[j]);
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

RESULT solve_sd(sd_t *s) {
  RESULT res = INVALID;
  if(!check_sd(s))return res=INVALID;
allocate:;
  sz_t no_hints = 0;
  cov_t cov = {
    .row = alloca(sizeof(val_t) * s->h),
    .col = alloca(sizeof(val_t) * s->w),
    .colfail = alloca(sizeof(sz_t) * s->w),
  };
  for(sz_t i=0;i<s->h;++i)cov.row[i]=ROWCOL;
  for(sz_t i=0;i<s->w;++i)cov.col[i]=s->ne2;
  for(sz_t i=0;i<s->w;++i)cov.colfail[i]=0;
  sol_t soln = {
    .row = alloca(sizeof(sz_t) * s->ne4),
    .col = alloca(sizeof(sz_t) * s->ne4),
  };
  val_t sdtable[s->ne4];
forward_knowns:;
  for(sz_t i = 0; i < s->ne4; ++i) {
    val_t t = s->table[i];
    if(t) {
      sd_update(s, &cov, i * s->ne2 + t - 1, FORWARD);
      ++no_hints;
    }
    soln.row[i] = soln.col[i] = UNDEF_SIZE,
      sdtable[i] = t;
  }
  ACTION action = FORWARD;
  min_t m = {
    .min = MINUNDEF,
    .fail_rate = UNDEF_SIZE,
    .min_col = 0,
  };
iterate_unknowns:;
  int i = 0;
  while(1) {
    while(i >= 0 && i < s->ne4 - no_hints) {
      if(action == FORWARD) {
        soln.col[i] = m.min_col;
        if(m.min > 1) {
          for(sz_t c = 0; c < s->w; ++c) {
            if(cov.col[c] < m.min || (cov.col[c] == m.min && cov.colfail[c] > m.fail_rate)) {
              m.min = cov.col[c];
              m.fail_rate = cov.colfail[c];
              m.min_col = c;
              soln.col[i] = c;
              if(m.min < 2)break;
            }
          }
        } else if(m.min == MINUNDEF) {
          ++cov.colfail[m.min_col],
          action = BACKTRACK,
          soln.row[i] = UNDEF_SIZE,
          --i;
        }
      }
      assert(i >= -1);
      sz_t
        cc = soln.col[(i==-1)?0:i],
        cr = soln.row[(i==-1)?0:i];
      assert(cc != UNDEF_SIZE);
      assert(cc < s->h);
      assert(cr == UNDEF_SIZE || cr < s->w);
      if(action == BACKTRACK && cr != UNDEF_SIZE)
        sd_update(s, &cov, RVAL(cc, cr), BACKTRACK);
      sz_t ir = (cr == UNDEF_SIZE) ? 0 : cr + 1;
      while(ir < s->ne2) {
        if(cov.row[RVAL(cc, ir)] == ROWCOL)break;
        ++ir;
      }
      if(ir < s->ne2)
        action = FORWARD,
        m = sd_update(s, &cov, RVAL(cc, ir), FORWARD),
        soln.row[i==-1?0:i] = ir,
        ++i;
      else {
        ++cov.colfail[m.min_col],
        action = BACKTRACK,
        soln.row[i==-1?0:i] = UNDEF_SIZE,
        --i;
      }
    }
    if(i < 0)break;
  get_sdtable:;
    for(int j = 0; j < i; ++j) {
      int r = RVAL(soln.col[j], soln.row[j]);
      assert(r < s->h);
      sdtable[r / s->ne2] = r % s->ne2 + 1;
    }
  change_res:;
    switch(res) {
      case INVALID:
        res = COMPLETE;
        memcpy(s->table, sdtable, sizeof(val_t) * s->ne4);
      break;
      case COMPLETE:
        res = MULTIPLE;
        goto endsolve;
      break;
      case MULTIPLE:
        assert(res != MULTIPLE);
      break;
    }
    --i, action = BACKTRACK;
  }
endsolve:
  return res;
}
