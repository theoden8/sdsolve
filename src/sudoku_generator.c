#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <getopt.h>
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

sdgen_t sdgen_init(sz_t n) {
  sdgen_t s={n=n,.ne2=n*n,.ne4=n*n*n*n,.solver=NULL,.table=malloc(sizeof(val_t)*n*n*n*n),
    .status=MULTIPLE,.no_vals=0};
  assert(s.table!=NULL),memset(s.table,0x00,sizeof(val_t)*s.ne4);
  return s;
}

void sdgen_free(sdgen_t s){if(s.solver)free_sd(s.solver);free(s.table);}

void sd_make_empty(sdgen_t *s) {
  for(sz_t i=0;i<s->ne4;++i)s->table[i]=0;
  s->no_vals=0;
}

void sd_init_diagonal_boxes(sdgen_t *s) {
  assert(s->no_vals == 0);
  sz_t ys[s->n];ord_arr(ys,s->n);shuffle_arr(ys,s->n);
  for(sz_t i = 0; i < s->n; ++i) {
    sd_fill_box(s, ys[i]*s->n+i);
  }
}

int chrlen(int x){return(x<10)?1:1+chrlen(x/10);}
void print_table(sdgen_t s) {
  printf("  %d\n", s.n);
  for(sz_t i=0;i<s.ne2;++i){for(sz_t j=0;j<s.ne2;++j){
    int x=s.table[i*s.ne2+j];
    int l=chrlen(x);for(int k=0;k<3-l;++k)putchar(' ');
    printf("%d",x);
  }putchar('\n');}
}

void setboard(sdgen_t *s) {
  if(s->solver==NULL)s->solver=make_sd(s->n,s->table);
  else sd_setboard(s->solver,s->table);
}

RESULT solve(sdgen_t *s) {
  setboard(s);
  s->status = solve_sd(s->solver);
  return s->status;
}

void complete(sdgen_t *s) {
  setboard(s);
  s->status = solve_sd(s->solver);
  for(int i=0;i<s->ne4;++i)s->table[i]=s->solver->table[i];
  s->no_vals=s->ne4;
}

void sd_fill_box(sdgen_t *s, sz_t i) {
  sz_t arr[s->ne2];
  ord_arr(arr,s->ne2),shuffle_arr(arr,s->ne2);
  for(sz_t j=0;j<s->ne2;++j)s->table[(s->n*(i/s->n)+(j/s->n))*s->ne2+(s->n*(i%s->n)+(j%s->n))]=arr[j]+1,++s->no_vals;
}

bool set_random(sdgen_t *s) {
  assert(s->no_vals != s->ne4);
  sz_t idx;
  do{idx=rand()%s->ne4;}while(s->table[idx]!=0);s->table[idx]=rand()%s->ne2+1,++s->no_vals;
  RESULT res = solve(s);
  if(res==INVALID){s->table[idx]=0,--s->no_vals;return set_random(s);}
  return res==COMPLETE;
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
  int choice;
  bool interactive = false;
  int timeout = -1;
  while (1)
  {
    static struct option long_options[] =
    {
      {"interactive", no_argument, 0, 'i'},
      {"timeout", required_argument, 0, 't'},
      {0,0,0,0}
    };
    int option_index = 0;
    choice = getopt_long( argc, argv, "it:", long_options, &option_index);
    if(choice == -1)
      break;
    switch( choice )
    {
      case 'i':
        interactive = true;
      break;
      case 't':
        if(!strcmp(optarg, "none")) {
          timeout=1e9;
          break;
        }
        timeout = atoi(optarg);
        if(timeout <= 0)fprintf(stderr, "error: invalid timeout %d\n",timeout),abort();
      break;
      case '?':
        /* getopt_long will have already printed an error */
        break;

      default:
        /* Not sure how to get here... */
        return EXIT_FAILURE;
    }
  }
  if(optind!=argc-1)return EXIT_FAILURE;
  seed_rng();
  sz_t n = atoi(argv[optind]);
  sdgen_t s = sdgen_init(n);
  sd_init_diagonal_boxes(&s);
  if(interactive)print_table(s);
  long c;
  /* c=clock(); */
  /* while(s.no_vals < s.ne4/5) { */
  /*   if(clock() - c > CLOCKS_PER_SEC * timeout)break; */
  /*   set_random(&s),print_table(s); */
  /* } */
  complete(&s);if(interactive)print_table(s);
  sz_t len=s.ne4,arr[len];ord_arr(arr, len);
  if(timeout==-1)timeout=s.ne2;
  while(1) {
    shuffle_arr(arr, len);
    c=clock();
    sz_t prev_len = len;
    for(sz_t i=0;i<len;++i) {
      assert(s.table[arr[i]]);
      if(clock() - c > CLOCKS_PER_SEC * timeout)
        goto endgen;
      if(try_unset(&s, arr[i])){shift_arr(arr,i--,&len)/**/;if(interactive)print_arr(arr,len),print_table(s);}
    }
    if(prev_len==len)goto endgen;
  }
endgen:;
  sz_t rename[s.ne2];ord_arr(rename,s.ne2),shuffle_arr(rename,s.ne2);
  for(sz_t i=0;i<s.ne4;++i)if(s.table[i])s.table[i]=rename[s.table[i]-1]+1;
  print_table(s);
  sdgen_free(s);
}
