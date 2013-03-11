#include <stdio.h>
#include <string.h>

static char usage[] =
"usage: permutations (-debug) (-optimize) set_size subset_size\n";

#define MAX_SET_SIZE 100
static int numerators[MAX_SET_SIZE];
static int denominators[MAX_SET_SIZE];

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  bool bOptimize;
  int num_optimize_divides;
  int set_size;
  int subset_size;
  int old_val;
  int numerator;
  int denominator;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bOptimize = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-optimize"))
      bOptimize = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&set_size);

  if (set_size > MAX_SET_SIZE) {
    printf("set_size must not exceed the maximum of %d\n",MAX_SET_SIZE);
    return 3;
  }

  sscanf(argv[curr_arg+1],"%d",&subset_size);

  if (subset_size > set_size) {
    printf("subset_size must be <= set_size\n");
    return 4;
  }

  for (n = 0; n < subset_size; n++) {
    numerators[n] = set_size - n;
    denominators[n] = subset_size - n;
  }

  if (bOptimize) {
    num_optimize_divides = 0;

    for (m = 0; m < subset_size; m++) {
      if (denominators[m] != 1) {
        for (n = 0; n < subset_size; n++) {
          if (!(numerators[n] % denominators[m])) {
            numerators[n] /= denominators[m];
            denominators[m] = 1;
            num_optimize_divides++;
            break;
          }
        }
      }
    }

    if (bDebug)
      printf("num_optimize_divides = %d\n",num_optimize_divides);
  }

  numerator = 1;
  denominator = 1;

  for (n = 0; n < subset_size; n++) {
    old_val = numerator;

    numerator *= numerators[n];

    if (old_val > numerator) {
      printf("overflow on numerator; n = %d\n",n);
      return 5;
    }

    old_val = denominator;

    denominator *= denominators[n];

    if (old_val > denominator) {
      printf("overflow on denominator; n = %d\n",n);
      return 6;
    }
  }

  printf("%d\n",numerator / denominator);

  return 0;
}
