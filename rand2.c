#include <stdio.h>
#include <stdlib.h>
 
#define MAX_SAMPLE_SIZE 1000
 
static int sample_hits[MAX_SAMPLE_SIZE];
static int random_sample_output[MAX_SAMPLE_SIZE];
 
int main(int argc,char **argv)
{
  int m;
  int n;
  int seed;
  int sample_size;
  int work;
  int curr_sample_size;
  int unused_count;
 
  if (argc != 3) {
    printf("usage: rand2 seed sample_size\n");
    return 1;
  }
 
  sscanf(argv[1],"%d",&seed);
  sscanf(argv[2],"%d",&sample_size);
 
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
