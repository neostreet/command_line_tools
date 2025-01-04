#include <stdio.h>
#include <stdlib.h>

static char malloc_failed[] = "malloc of %d bytes failed\n";

int populate_random_sample_ixs(int sample_size,int **random_sample_ixs_ptr);
 
int main(int argc,char **argv)
{
  int n;
  int seed;
  int sample_size;
  int retval;
  int *random_sample_ixs;
 
  if (argc != 3) {
    printf("usage: rand6 seed sample_size\n");
    return 1;
  }
 
  sscanf(argv[1],"%d",&seed);
  sscanf(argv[2],"%d",&sample_size);
 
  srand(seed);

  retval = populate_random_sample_ixs(sample_size,&random_sample_ixs);

  if (!retval) {
    for (n = 0; n < sample_size; n++) {
      printf("%d\n",random_sample_ixs[n]);
    }
  }
 
  return 0;
}

int populate_random_sample_ixs(int sample_size,int **random_sample_ixs_ptr)
{
  int m;
  int n;
  int work;
  int mem_amount;
  int *random_sample_ixs;
  int *random_sample_hits;
  int curr_sample_size;
  int unused_count;

  mem_amount = sample_size * sizeof(int *);

  if ((random_sample_ixs = (int *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    return 1;
  }

  if ((random_sample_hits = (int *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    return 2;
  }

  curr_sample_size = sample_size;

  for (n = 0; n < sample_size; n++)
    random_sample_hits[n] = 0;

  for (n = 0; n < sample_size; n++) {
    work = rand();
    work %= curr_sample_size;
    work++;
 
    unused_count = 0;
 
    for (m = 0; m < sample_size; m++) {
      if (!random_sample_hits[m]) {
        unused_count++;
 
        if (unused_count == work) {
          random_sample_hits[m] = 1;
          break;
        }
      }
    }
 
    random_sample_ixs[n] = m;
 
    curr_sample_size--;
  }

  free(random_sample_hits);
  *random_sample_ixs_ptr = random_sample_ixs;
 
  return 0;
}
