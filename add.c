#include <stdio.h>

static char usage[] = "usage: add addend addend (addend ...)\n";

int main(int argc,char **argv)
{
  int n;
  double work;
  double total;

  if (argc < 3) {
    printf(usage);
    return 1;
  }

  total = (double)0;

  for (n = 1; n < argc; n++) {
    sscanf(argv[n],"%lf",&work);

    total += work;
  }

  printf("%lf\n",total);

  return 0;
}
