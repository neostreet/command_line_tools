#include <stdio.h>

static char usage[] = "usage: muldiv multiplicand multiplicand divisor\n";

int main(int argc,char **argv)
{
  double multiplicand1;
  double multiplicand2;
  double divisor;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%lf",&multiplicand1);
  sscanf(argv[2],"%lf",&multiplicand2);
  sscanf(argv[3],"%lf",&divisor);

  printf("%lf\n",multiplicand1 * multiplicand2 / divisor);

  return 0;
}
