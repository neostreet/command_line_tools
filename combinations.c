#include <stdio.h>
#include <string.h>

static char usage[] =
"usage: combinations (-debug) set_size subset_size\n";

#define MAX_SET_SIZE 500
static int numerators[MAX_SET_SIZE];

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int set_size;
  int subset_size;
  int old_val;
  int numerator;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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
  }

  numerator = 1;

  for (n = 0; n < subset_size; n++) {
    old_val = numerator;

    numerator *= numerators[n];

    if (old_val > numerator) {
      printf("overflow on numerator; n = %d\n",n);
      return 5;
    }
  }

  printf("%d\n",numerator);

  return 0;
}
