#include <stdio.h>

static char usage[] = "usage: div dividend divisor\n";

int main(int argc,char **argv)
{
  int n;
  double dividend;
  double divisor;
  double quotient;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%lf",&dividend);
  sscanf(argv[2],"%lf",&divisor);

  quotient = dividend / divisor;

  printf("%lf\n",quotient);

  return 0;
}
