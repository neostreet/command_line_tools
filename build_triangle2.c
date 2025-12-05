#include <stdio.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: build_triangle2 left top width height color\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int parms[NUM_PARMS];
  int middle;
  int left_edge;
  int right_edge;

  if (argc != 6) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[1 + n],"%d",&parms[n]);

  middle = parms[WIDTH] / 2;
  left_edge = middle;
  right_edge = middle;

  for (n = 0; n < NUM_PARMS; n++)
    printf("%d\n",parms[n]);

  for (m = 0; m < parms[HEIGHT]; m++) {
    if (m == parms[HEIGHT] - 1) {
      for (n = 0; n < parms[WIDTH]; n++)
        printf("%s\n",argv[5]);
    }
    else if (m % 2) {
      for (n = 0; n < parms[WIDTH]; n++)
        printf("00ffffff\n");

      left_edge--;
      right_edge++;

      continue;
    }

    for (n = 0; n < parms[WIDTH]; n++) {
      if (!m) {
        if (n == middle)
          printf("%s\n",argv[5]);
        else
          printf("00ffffff\n");
      }
      else {
        if ((n == left_edge) || (n == right_edge))
          printf("%s\n",argv[5]);
        else
          printf("00ffffff\n");
      }
    }
  }

  return 0;
}
