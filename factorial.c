#include <stdio.h>
#include <string.h>

static char usage[] = "usage: factorial (-debug) val\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int val;
  int old_val;
  int factorial;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&val);

  factorial = val;
  old_val = factorial;

  for (n = val - 1; (n > 1); n--,old_val = factorial){
    factorial *= n;

    if (factorial < old_val) {
      printf("overflow; n = %d\n",n);
      return 3;
    }
  }

  printf("%d\n",factorial);

  return 0;
}
