#include <stdio.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: build_triangle left top width height color\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int parms[NUM_PARMS];

  if (argc != 6) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[1 + n],"%d",&parms[n]);

  for (n = 0; n < NUM_PARMS; n++)
    printf("%d\n",parms[n]);

  for (m = 0; m < parms[HEIGHT]; m++) {
    for (n = 0; n < parms[WIDTH]; n++) {
      if (!n || (n == m) || (m == parms[HEIGHT] - 1))
        printf("%s\n",argv[5]);
      else
        printf("00ffffff\n");
    }
  }

  return 0;
}
