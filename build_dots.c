#include <stdio.h>

static char usage[] = "usage: build_dots width height\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int width;
  int height;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&width);
  sscanf(argv[2],"%d",&height);

  for (m = 0; m < height; m++) {
    for (n = 0; n < width; n++) {
      putchar('.');

      if (n < width - 1)
        putchar(' ');
      else
        putchar(0x0a);
    }
  }

  return 0;
}
