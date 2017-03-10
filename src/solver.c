#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "solver_io.h"
#include "solver.h"

void solve_sudoku(sudoku_t **s) {
  if(!is_valid_sudoku(*s)) {
    (*s)->res = INVALID;
    return;
  } else if((*s)->no_virts == 0) {
    (*s)->res = COMPLETE;
    return;
  }
  if((*s)->depth == 0) {
    sudoku_iterate_known_values(*s);
    if((*s)->no_virts == 0 && is_valid_sudoku(*s)) {
      (*s)->res = COMPLETE;
      return;
    } else if((*s)->no_virts == 0) {
      (*s)->res = INVALID;
      return;
    }
  }
  recurse_sudoku(s, find_best_recurse_point(*s));
  assert((*s)->res != INCOMPLETE);
  if((*s)->res == COMPLETE && !is_valid_sudoku(*s)) {
    (*s)->res = INVALID;
    return;
  } else if((*s)->res == COMPLETE) {
    assert(is_valid_sudoku(*s));
  }
}

// short-circuit check for completeness
bool is_completed_sudoku(const sudoku_t *s) {
  return !s->no_virts && is_valid_sudoku(s);
}

// checks whether the given sudoku is valid
bool is_valid_sudoku(const sudoku_t *s) {
  char values[s->n_e2 * 3];
  for(sz_t i = 0; i < s->n_e2; ++i) {
    memset(values, false, s->n_e2 * 3 * sizeof(char));
    for(sz_t j = 0; j < s->n_e2; ++j) {
      sz_t pos[3] = {
        make_pos_yx(s, i, j),
        make_pos_xy(s, i, j),
        make_pos_yx(s, s->n * (i / s->n) + (j / s->n), s->n * (i % s->n) + (j % s->n)),
      };
      for(int k = 0; k < 3; ++k) {
        sz_t p = pos[k];
        val_t t = get_sudoku_val(s, p);
        if(!t)
          continue;
        if(values[k*s->n_e2+t-1])
          return false;
        values[k*s->n_e2+t-1]=true;
      }
    }
  }
  return true;
}

// pipe the results from recursed sudoku to the current solution
sudoku_res_t percept_recurse_results(sudoku_res_t cur, sudoku_res_t future) {
  assert(cur != INVALID);
  assert(future != INCOMPLETE);
  switch(cur) {
    case INCOMPLETE:
      switch(future) {
        case INVALID: return INCOMPLETE;
        case COMPLETE: return COMPLETE;
        case MULTIPLE: return MULTIPLE;
      }
    break;
    case COMPLETE:
      switch(future) {
        case INVALID:
          return COMPLETE;
        case COMPLETE: case MULTIPLE:
          return MULTIPLE;
      }
    break;
    case MULTIPLE:
    return MULTIPLE;
  }
  exit(1);
}

void recurse_sudoku(sudoku_t **s, sz_t pos) {
  static int depth = 0;
  sudoku_t *solution = NULL; // solution board
  val_t r; // rightmost possible value
  val_t no_virts_pos = count_sudoku_virts(*s, pos);
  sudoku_res_t result = INCOMPLETE;
  for(val_t i = 0; i < no_virts_pos; ++i) { // iterate possibilities
    r = get_sudoku_rightmost_virt(*s, pos);
    assert(r != 0);
    ++depth;
    sudoku_t *assumpt = cpy_sudoku(*s); // make the assumption sudoku
    assumpt->depth = depth;
    set_sudoku_value(assumpt, pos, r); // and change the value in it
    solve_sudoku(&assumpt); // solve it
    result = percept_recurse_results(result, assumpt->res); // consider the result
    if(result == COMPLETE && assumpt->res == COMPLETE) {
      // if it looks good, remember the solution
      assert(solution == NULL);
      solution = cpy_sudoku(assumpt);
    } else if(result == MULTIPLE) {
      // if it looks good again, forget the solution and break
      if(solution != NULL)
        free_sudoku(&solution);
      assert(solution == NULL);
      result = MULTIPLE;
      free_sudoku(&assumpt);
      unset_sudoku_virt_d(*s, pos, r);
      --depth;
      break;
    }
    free_sudoku(&assumpt);
    unset_sudoku_virt_d(*s, pos, r);
    --depth;
  }
  if(solution != NULL)
    assert(result == COMPLETE);
  // if we didnt recurse anything
  if(result == INCOMPLETE)
    result = INVALID;
  // then incomplete is impossible by this line
  assert(result != INCOMPLETE);
  (*s)->res = result;
  // if it looked good once only, we must have remembered the solution
  if((*s)->res == COMPLETE) {
    // now we forget our current sudoku and move the pointer, thus avoiding
    // assignment operation
    assert(solution != NULL);
    free_sudoku(s);
    *s = solution;
  }
}

// find the point with least possibilities
sz_t find_best_recurse_point(const sudoku_t *s) {
  // we shouldnt recurse if we have found something about this sudoku
  assert(s->res == INCOMPLETE);
  val_t no_bits = s->n_e2 + 1; // minimal number of possibilities
  sz_t best_pos = 0xffff; // i.e. -1 short
  for(sz_t pos = 0; pos < s->n_e4; ++pos) {
    if(is_defined_val(s, pos))
      continue;
    val_t new_no_bits = count_sudoku_virts(s, pos);
    if(new_no_bits < no_bits) {
      no_bits = new_no_bits,
      best_pos = pos;
      if(no_bits == 2)
        break;
    }
  }
  assert(best_pos != 0xffff);
  return best_pos;
}

// eliminate all possibilities in the first run of solve_sudoku
void sudoku_iterate_known_values(sudoku_t *s) {
  for(sz_t y = 0; y < s->n_e2; ++y) {
    for(sz_t x = 0; x < s->n_e2; ++x) {
      sz_t pos = make_pos_xy(s, x, y);
      if(s->no_virts == 0) {
        s->res = COMPLETE;
        return;
      }
      if(is_defined_val(s, pos)) {
        val_t t = get_sudoku_val(s, pos);
        eliminate_value(s, pos);
      }
    }
  }
}

// more high-level than unset_sudoku_virt_dd: if only one virtual value left, it
// will set the actual value and perform the elimination
void unset_possible_val_d(sudoku_t *s, sz_t pos, val_t t) {
  assert(t != 0);
  if(!is_defined_val(s, pos)) {
    unset_sudoku_virt_d(s, pos, t);
    val_t f = find_unique_virt_val(s, pos);
    if(f == 255) {
      s->res = INVALID;
      return;
    }
    if(f)
      set_sudoku_value(s, pos, f);
  }
}

// set the actual value and perform elimination
void set_sudoku_value(sudoku_t *s, sz_t pos, val_t x) {
  assert(x != 0);
  set_sudoku_val_d(s, pos, x);
  eliminate_value(s, pos);
}

// eliminate the value in the position from its row, column and box
static void eliminate_value(sudoku_t *s, sz_t pos) {
  val_t t = get_sudoku_val(s, pos);
  assert(t != 0);
  sz_t
    row = get_pos_y(s, pos),
    col = get_pos_x(s, pos),
    box = get_pos_box(s, pos);
  for(sz_t i = 0; i < s->n_e2; ++i) {
    unset_possible_val_d(s, make_pos_yx(s, row, i), t);
    unset_possible_val_d(s, make_pos_xy(s, col, i), t);
    unset_possible_val_d(s, make_pos_yx(s, s->n * (box / s->n) + (i / s->n), s->n * (box % s->n) + (i % s->n)), t);
  }
}
