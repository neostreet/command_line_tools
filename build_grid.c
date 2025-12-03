#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: build_grid left top width height color1 color2\n";
static char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int nobs;
  int parms[NUM_PARMS];
  COLORREF colors[2];

  if (argc != 7) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[1 + n],"%d",&parms[n]);

  nobs = parms[WIDTH] * parms[HEIGHT];

  for (n = 0; n < NUM_PARMS; n++)
    printf("%d\n",parms[n]);

  for (n = 0; n < nobs; n++)
    printf("%s\n",!(n % 2) ? argv[5] : argv[6]);

  return 0;
}
