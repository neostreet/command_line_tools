#include <windows.h>
#include <stdio.h>
#include <string.h>

#define LEFT      0
#define TOP       1
#define NUM_PARMS 2

static char usage[] = "usage: scr_grab_suit (-debug) left top\n";

static char *suits[] = {
  "clubs",
  "diamonds",
  "hearts",
  "spades"
};
#define NUM_SUITS (sizeof suits / sizeof (char *))

static int suit_colors[] = {
  0x90909,
  0xfafafe,
  0xffffff,
  0xe8e8e8
};

static char couldnt_open[] = "couldn't open %s\n";
static char fmt[] = "%08x\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int parms[NUM_PARMS];
  HDC hDC;
  COLORREF color;

  if ((argc < 3) || (argc > 4)) {
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

  if (argc - curr_arg != 2) {
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

  if (bDebug) {
    printf("parms[LEFT]   = %d\n",parms[LEFT]);
    printf("parms[TOP]    = %d\n",parms[TOP]);
  }

  color = GetPixel(hDC,parms[LEFT],parms[TOP]);

  if (bDebug)
    printf("color = %x\n",color);

  for (n = 0; n < NUM_SUITS; n++) {
    if (color == suit_colors[n])
      break;
  }

  if (n == NUM_SUITS) {
    printf("couldn't determine suit\n");

    return 4;
  }

  printf("%s\n",suits[n]);

  return 0;
}
