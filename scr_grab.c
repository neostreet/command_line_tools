#include <windows.h>
#include <stdio.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char couldnt_open[] = "couldn't open %s\n";
static char fmt[] = "%08x\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int parms[NUM_PARMS];
  HDC hDC;
  COLORREF color;
  FILE *fptr;

  if (argc != 6) {
    printf("usage: scr_grab left top width height outfile\n");
    return 1;
  }

  hDC = GetDC(NULL);

  if (!hDC) {
    printf("GetDC failed for screen\n");
    return 2;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[1 + n],"%d",&parms[n]);

  if ((fptr = fopen(argv[5],"w")) == NULL) {
    printf(couldnt_open,argv[5]);
    return 3;
  }

  for (m = 0; m < NUM_PARMS; m++)
    fprintf(fptr,"%d\n",parms[m]);

  for (m = 0; m < parms[HEIGHT]; m++) {
    for (n = 0; n < parms[WIDTH]; n++) {
      color = GetPixel(hDC,parms[LEFT] + n,parms[TOP] + m);
      fprintf(fptr,fmt,color);
    }
  }

  fclose(fptr);

  return 0;
}
