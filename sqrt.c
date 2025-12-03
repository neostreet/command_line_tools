#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static char usage[] = "usage: sqrt val (val ...)\n";

int main(int argc,char **argv)
{
  int n;
  double val;
  double sqrt1;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (n = 1; n < argc; n++) {
    sscanf(argv[n],"%lf",&val);
    sqrt1 = sqrt(val);
    printf("%lf: %lf\n",val,sqrt1);
  }

  return 0;
}
