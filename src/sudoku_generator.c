#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "sudoku_generator.h"

void seed_rng() {
  int fp = open("/dev/random", O_RDONLY);
  if(fp == -1)abort();
  unsigned seed, pos = 0;
  while (pos < sizeof(seed)) {
    int amt = read(fp, (char *) &seed + pos, sizeof(seed) - pos);
    if(amt <= 0) abort(); pos += amt;
  }
  srand(seed),close(fp);
}

void sd_init_diagonal_boxes(sdgen_t *s) {
  assert(s->no_vals == 0);
  for(sz_t i = 0; i < s->n; ++i) {
    sz_t topleft = i * (s->n*s->ne2 + s->n);
    s->table[topleft] = rand() % s->ne2 + 1;
  }
}

sdgen_t sdgen_init(sz_t n) {
  sdgen_t s={.n=n,.ne2=n*n,.ne4=n*n*n*n,.table=malloc(sizeof(val_t)*n*n*n*n),.status=MULTIPLE,.no_vals=0};
  assert(s.table!=NULL),memset(s.table,0x00,sizeof(val_t)*s.ne4);
  return s;
}

void sdgen_free(sdgen_t s){free(s.table);}

int chrlen(int x){return(x<10)?1:1+chrlen(x/10);}
void print_table(sdgen_t s) {
  printf("  %d\n", s.n);
  for(sz_t i=0;i<s.ne2;++i){for(sz_t j=0;j<s.ne2;++j){
    int x=s.table[i*s.ne2+j];
    int l=chrlen(x);for(int k=0;k<3-l;++k)putchar(' ');
    printf("%d",x);
  }putchar('\n');}
}

bool solve(sdgen_t *s) {
  sd_t *solver = make_sd(s->n, s->table);
  s->status = solve_sd(solver);
  free_sd(solver);
  return s->status != INVALID;
}

void complete(sdgen_t *s) {
  sd_t *solver = make_sd(s->n, s->table);
  s->status = solve_sd(solver);
  for(int i=0;i<s->ne4;++i)s->table[i]=solver->table[i];
  s->no_vals=s->ne4;
  free_sd(solver);
}

void set_random(sdgen_t *s) {
  assert(s->no_vals != s->ne4);
  sz_t idx;
  do{idx=rand()%s->ne4;}while(s->table[idx]!=0);s->table[idx]=rand()%s->ne2+1,++s->no_vals;
  if(!solve(s)){s->table[idx]=0,--s->no_vals,set_random(s);return;}
  /* printf("%lu\n", s->no_vals); */
}

bool try_unset(sdgen_t *s, sz_t idx) {
  val_t t=s->table[idx];
  assert(t);
  assert(s->no_vals);
  s->table[idx]=0,solve(s),assert(s->status != INVALID);
  if(s->status == MULTIPLE){s->table[idx]=t;return false;}
  --s->no_vals;
  return true;
}

static void print_arr(sz_t *arr, sz_t len){
  printf("len==%d\n", len);
  for(sz_t i=0;i<len;++i)printf("%d ", arr[i]); putchar('\n');
}
static void ord_arr(sz_t *arr, sz_t len){for(sz_t i=0;i<len;++i)arr[i]=i;}
static void shuffle_arr(sz_t *arr, sz_t len) {
  for(sz_t i = 1; i < len; ++i) {
    sz_t j = rand()%i;
    sz_t tmp;if(i!=j)tmp=arr[i],arr[i]=arr[j],arr[j]=tmp;
  }
}
static void shift_arr(sz_t *arr, sz_t idx, sz_t *len) {
  for(sz_t i=idx; i<*len-1; ++i)
    arr[i]=arr[i+1];
  --*len;
}

main(int argc, char *argv[]) {
  if(argc != 2)return EXIT_FAILURE;
  seed_rng();
  sz_t n = atoi(argv[1]);
  sdgen_t s = sdgen_init(n);
  sd_init_diagonal_boxes(&s);
  /* print_table(s); */
  long c;
  c=clock();
  while(s.no_vals < s.ne4/4){
    if(clock() - c > CLOCKS_PER_SEC * s.ne2)
      break;
    set_random(&s);
    /* print_table(s); */
  }
  complete(&s);
  /* print_table(s); */
  sz_t len = s.ne4;
  sz_t arr[s.ne4];ord_arr(arr, len);
  while(1) {
    shuffle_arr(arr, len);
    c=clock();
    sz_t prev_len = len;
    for(sz_t i=0;i<len;++i) {
      assert(s.table[arr[i]]);
      if(clock() - c > CLOCKS_PER_SEC * s.ne2)
        goto endgen;
      if(try_unset(&s, arr[i]))shift_arr(arr,i--,&len)/*,print_arr(arr,len),print_table(s)*/;
    }
    if(prev_len==len)goto endgen;
  }
endgen:
  print_table(s);
  sdgen_free(s);
}
