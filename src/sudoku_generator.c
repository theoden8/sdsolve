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

sdgen_t create_empty_table(sz_t n) {
  sdgen_t s={.n=n,.ne2=n*n,.ne4=n*n*n*n,.table=malloc(sizeof(val_t)*n*n*n*n),.status=MULTIPLE,.no_vals=0};
  assert(s.table!=NULL),memset(s.table,0x00,sizeof(val_t)*s.ne4);return s;
}

void free_table(sdgen_t s){free(s.table);}

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

void unset_random(sdgen_t *s) {
  /* printf("%lu\n", s->no_vals); */
  assert(s->no_vals != 0);
  sz_t idx;val_t t;
  do{idx=rand()%s->ne4;}while(s->table[idx]==0);t=s->table[idx],s->table[idx]=0;
  solve(s);
  assert(s->status!=INVALID);
  if(s->status==MULTIPLE){s->table[idx]=t;return;}
  --s->no_vals;
}

main(int argc, char *argv[]) {
  if(argc != 2)return EXIT_FAILURE;
  seed_rng();
  sz_t n = atoi(argv[1]);
  sdgen_t s = create_empty_table(n);
  long c;
  c=clock();
  while(s.no_vals < s.ne4/4){
    if(clock() - c > CLOCKS_PER_SEC * s.ne2)
      break;
    set_random(&s);
  }
  complete(&s);
  c=clock();
  for(sz_t i=0;i<s.ne4;++i) {
    if(clock() - c > CLOCKS_PER_SEC * s.ne2)
      break;
    unset_random(&s);
    //printf("..%d\t", s.ne4-i);
  }
  print_table(s);
  free_table(s);
}
