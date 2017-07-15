#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <alloca.h>
#include <stdbool.h>
#include <math.h>

#include "algx.h"

// the implementation idea is taken (mainly) from
//   https://raw.githubusercontent.com/attractivechaos/plb/master/sudoku/sudoku_v1.c
// fully written from scratch

const sz_t UNDEF_SIZE = -1;

sd_t *make_sd(sz_t n, val_t *table) {
attributes:;
  sd_t *s=malloc(sizeof(sd_t));assert(s != NULL);
  s->n=n, s->ne2=n*n, s->ne3=s->ne2*n, s->ne4=s->ne3 * n;
  s->table=malloc(sizeof(val_t)*s->ne4),assert(s->table!=NULL);
  sd_setboard(s, table);
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

static inline min_t default_min(const sd_t *s) {
  return (min_t){
    .min = MINUNDEF,
    .min_col = 0,
    .fail_rate = 0,
    .choice_rate = 0,
  };
}

static inline min_t sd_update(const sd_t *s, sz_t r, ACTION flag) {
  min_t m = default_min(s);
  const static val_t LBIT = 1 << (CHAR_BIT * sizeof(val_t) - 1);
  for(sz_t ic = 0; ic < NO_CONSTR; ++ic)s->cov->col[C_CNSTR(r, ic)] ^= LBIT;
  for(sz_t ic = 0; ic < NO_CONSTR; ++ic)
    if(flag==FORWARD)sd_forward(s,r,ic,&m);else
      sd_backtrack(s,r,ic);
  return m;
}

static inline void sd_forward(const sd_t *s, sz_t r, sz_t ic, min_t *m) {
  assert(ic < NO_CONSTR);
  const sz_t c = C_CNSTR(r, ic);assert(c < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = R_SLNS(c, ir); assert(rr < s->h);
    if(s->cov->row[rr]++ != 0)continue;
    for(sz_t ic2 = 0; ic2 < NO_CONSTR; ++ic2) {
      sz_t cc = C_CNSTR(rr, ic2); assert(cc < s->w);
      if(--s->cov->col[cc] < m->min)
        m->min=s->cov->col[cc],m->min_col=cc,
        m->fail_rate=s->cov->colfail[cc],
        m->choice_rate=s->cov->colchoice[cc];
    }
  }
}

static inline void sd_backtrack(const sd_t *s, sz_t r, sz_t ic) {
  assert(ic < NO_CONSTR);
  sz_t c = C_CNSTR(r, ic); assert(c < s->w);
  for(sz_t ir = 0; ir < s->ne2; ++ir) {
    sz_t rr = R_SLNS(c, ir);
    assert(rr < s->h);
    --s->cov->row[rr];
    assert(0 <= s->cov->row[rr] && s->cov->row[rr] <= NO_CONSTR);
    if(s->cov->row[rr] != ROWCOL)continue;
    sz_t *it = &C_CNSTR(rr, 0);
    ++s->cov->col[it[ROWCOL]], ++s->cov->col[it[BOXNUM]],
    ++s->cov->col[it[ROWNUM]], ++s->cov->col[it[COLNUM]];
  }
}

static inline bool check_sd(sd_t *s) {
  val_t *check = alloca(s->ne2*3);
  assert(check != NULL);
  for(sz_t i = 0; i < s->ne2; ++i) {
    memset(check, 0x00, s->ne2 * 3 * sizeof(val_t));
    for(sz_t j = 0; j < s->ne2; ++j) {
      sz_t pos[3]={i*s->ne2+j, j*s->ne2+i, (i/s->n*s->n+j/s->n)*s->ne2+(i%s->n)*s->n+j%s->n};
      for(int k = 0; k < 3; ++k) {
        val_t t=s->table[pos[k]];if(!t)continue;
        val_t*chk=&check[k*s->ne2+t-1]; if(*chk)
          return false ; *chk=true;
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
  s->i = 0;
}

static inline void sd_forward_knowns(sd_t *s) {
  sd_reset(s);
  for(sz_t i = 0; i < s->ne4; ++i) {
    val_t t = s->table[i];
    if(t) {
      sd_update(s, i * s->ne2 + t - 1, FORWARD);
      ++s->no_hints;
    }
    s->soln->row[i]=s->soln->col[i]=UNDEF_SIZE, s->buf[i]=t;
  }
  s->no_vars = s->ne4 - s->no_hints;
}

void sd_setboard(sd_t *s, val_t *table) {
  memcpy(s->table, table, sizeof(val_t) * s->ne4);
}

RESULT solve_sd(sd_t *s) {
  RESULT res = INVALID;
  if(!check_sd(s))return res=INVALID;
presetup:;
  sd_forward_knowns(s);
  ACTION action = FORWARD;
  min_t m = default_min(s);
iterate_unknowns:;
  while(1) {
    while(s->i >= 0 && s->i < s->no_vars) {
      if(action == FORWARD) {
        s->soln->col[s->i] = m.min_col;
        if(m.min > 1) {
          for(sz_t c = 0; c < s->w; ++c) {
            if(s->cov->col[c] < m.min || (s->cov->col[c] == m.min && s->cov->colfail[c] > m.fail_rate)) {
              m.min=s->cov->col[c],m.min_col=c,
              m.fail_rate=s->cov->colfail[c],
              m.choice_rate=s->cov->colchoice[c],
              s->soln->col[s->i]=c;if(m.min<2)break;
            }
          }
        } else if(m.min == MINUNDEF) {
          action = BACKTRACK,
          s->cov->colfail[m.min_col] = s->cov->colchoice[m.min_col],
          s->soln->row[s->i] = UNDEF_SIZE,
          --s->i;
        }
      }
      assert(s->i >= -1);
      const int_fast32_t ii = (s->i == -1) ? 0 : s->i;
      const sz_t cc = s->soln->col[ii], cr = s->soln->row[ii];
      assert(cc != UNDEF_SIZE),assert(cc < s->h),assert(cr == UNDEF_SIZE || cr < s->w);
      if(action == BACKTRACK && cr != UNDEF_SIZE)
        s->cov->colfail[cc] = s->cov->colchoice[cc],
        sd_update(s, R_SLNS(cc, cr), BACKTRACK);
      val_t ir = (cr == UNDEF_SIZE) ? 0 : cr + 1;
      while(ir < s->ne2) {
        if(s->cov->row[R_SLNS(cc, ir)] == 0)break;
        ++ir;
      }
      if(ir < s->ne2) {
        action = FORWARD;
        const sz_t diff=s->ne2-s->cov->col[cc];
        s->cov->colchoice[cc] += diff*diff*(s->no_vars-s->i) / s->w + 1,
        /* ++s->cov->colchoice[cc], */
        m = sd_update(s, R_SLNS(cc, ir), FORWARD),
        s->soln->row[ii] = ir,
        ++s->i;
      } else {
        action = BACKTRACK,
        s->cov->colfail[cc] = s->cov->colchoice[cc],
        s->soln->row[ii] = UNDEF_SIZE,
        --s->i;
      }
    }
    if(s->i < 0)break;
  get_sdtable:;
  change_res:;
    switch(res) {
      case INVALID:
        res = COMPLETE;
        for(sz_t j = 0; j < s->i; ++j) {
          sz_t r = R_SLNS(s->soln->col[j], s->soln->row[j]);
          assert(r < s->h);
          s->buf[r / s->ne2] = r % s->ne2 + 1;
        }
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
    --s->i,action=BACKTRACK;if(s->i>=0)
      s->cov->colfail[s->soln->col[s->i]] = s->cov->colchoice[s->soln->col[s->i]];
  }
endsolve:
  return res;
}
