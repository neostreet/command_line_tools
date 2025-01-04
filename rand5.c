#include <stdio.h>
#include <stdlib.h>
#include <time.h>
 
#define MAX_SAMPLE_SIZE 1000
 
static int sample_hits[MAX_SAMPLE_SIZE];
static int random_sample_output[MAX_SAMPLE_SIZE];
 
int main(int argc,char **argv)
{
  int m;
  int n;
  time_t now;
  int seed;
  int sample_size;
  int work;
  int curr_sample_size;
  int unused_count;
 
  if (argc != 2) {
    printf("usage: rand5 sample_size\n");
    return 1;
  }

  time(&now);
  seed = (int)now;
  printf("seed = %d\n",seed);

  sscanf(argv[1],"%d",&sample_size);
 
  if (sample_size > MAX_SAMPLE_SIZE) {
    printf("sample_size > MAX_SAMPLE_SIZE (%d)\n",MAX_SAMPLE_SIZE);
    return 2;
  }
 
  srand(seed);
 
  curr_sample_size = sample_size;
 
  for (n = 0; n < sample_size; n++) {
    work = rand();
    work %= curr_sample_size;
    work++;
 
    unused_count = 0;
 
    for (m = 0; m < sample_size; m++) {
      if (!sample_hits[m]) {
        unused_count++;
 
        if (unused_count == work) {
          sample_hits[m] = 1;
          break;
        }
      }
    }
 
    random_sample_output[n] = m;
 
    curr_sample_size--;
  }
 
  for (n = 0; n < sample_size; n++) {
    printf("%d\n",random_sample_output[n]);
  }
 
  return 0;
}
