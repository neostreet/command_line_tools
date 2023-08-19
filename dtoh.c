#include <stdio.h>

int main(int argc,char **argv)
{
  int n;
  long l;

  for (n = 1; n < argc; n++) {
    sscanf(argv[n],"%ld",&l);
    printf("%lx\n",l);
  }

  return 0;
}
