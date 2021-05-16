#include <stdio.h>

int main(int argc,char **argv)
{
  int cur_arg;
  double multiplicand;
  double product;

  if (argc < 3) {
    printf("usage: mul multiplicand multiplicand (multiplicand ...)\n");
    return 1;
  }

  product = 1.0;

  for (cur_arg = 1; cur_arg < argc; cur_arg++) {
    sscanf(argv[cur_arg],"%lf",&multiplicand);
    product *= multiplicand;
  }

  printf("%lf\n",product);

  return 1;
}
