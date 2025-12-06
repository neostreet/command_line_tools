#include <stdio.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: build_grid left top width height color\n";
static char couldnt_open[] = "couldn't open %s\n";

static char white[] = "00ffffff";

int main(int argc,char **argv)
{
  int n;
  int nobs;
  int parms[NUM_PARMS];

  if (argc != 6) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[1 + n],"%d",&parms[n]);

  nobs = parms[WIDTH] * parms[HEIGHT];

  for (n = 0; n < NUM_PARMS; n++)
    printf("%d\n",parms[n]);

  for (n = 0; n < nobs; n++)
    printf("%s\n",!(n % 2) ? argv[5] : white);

  return 0;
}
