#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: scr_grab (-debug) left top width height outfile\n";

static char couldnt_open[] = "couldn't open %s\n";
static char fmt[] = "%08x\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  int parms[NUM_PARMS];
  HDC hDC;
  COLORREF color;
  FILE *fptr;
  time_t start_time;
  time_t end_time;

  if ((argc < 6) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 5) {
    printf(usage);
    return 2;
  }

  hDC = GetDC(NULL);

  if (!hDC) {
    printf("GetDC failed for screen\n");
    return 3;
  }

  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[curr_arg + n],"%d",&parms[n]);

  if ((fptr = fopen(argv[argc - 1],"w")) == NULL) {
    printf(couldnt_open,argv[5]);
    return 4;
  }

  if (bDebug) {
    printf("parms[LEFT]   = %d\n",parms[LEFT]);
    printf("parms[TOP]    = %d\n",parms[TOP]);
    printf("parms[WIDTH]  = %d\n",parms[WIDTH]);
    printf("parms[HEIGHT] = %d\n",parms[HEIGHT]);
  }

  time(&start_time);

  for (m = 0; m < NUM_PARMS; m++)
    fprintf(fptr,"%d\n",parms[m]);

  for (m = 0; m < parms[HEIGHT]; m++) {
    for (n = 0; n < parms[WIDTH]; n++) {
      color = GetPixel(hDC,parms[LEFT] + n,parms[TOP] + m);

      if (bDebug)
        printf("color = %x\n",color);

      fprintf(fptr,fmt,color);
    }
  }

  fclose(fptr);

  time(&end_time);

  printf("\nelapsed time: %d seconds\n",end_time - start_time);

  return 0;
}
