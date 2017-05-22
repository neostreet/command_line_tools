#include <stdio.h>

int main(int argc,char **argv)
{
  int n;
  int nobs;
  int color;
  int width;
  int height;

  if (argc != 4) {
    printf("usage: make_color color width height\n");
    return 1;
  }

  sscanf(argv[1],"%x",&color);
  sscanf(argv[2],"%d",&width);
  sscanf(argv[3],"%d",&height);

  printf("353\n");
  printf("402\n");
  printf("60\n");
  printf("84\n");

  nobs = width * height;

  for (n = 0; n < nobs; n++)
    printf("%x\n",color);

  return 0;
}
