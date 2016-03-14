#include <windows.h>
#include <stdio.h>
#include <string.h>

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define NUM_PARMS 3

static char usage[] =
"usage: scr_grab_board (-debug) (-abbrev) left top width\n";

static char *suits[] = {
  "clubs",
  "diamonds",
  "hearts",
  "spades"
};
#define NUM_SUITS (sizeof suits / sizeof (char *))

static char *suit_abbrevs = "cdhs";

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
  int m;
  int n;
  int curr_arg;
  bool bDebug;
  bool bAbbrev;
  int parms[NUM_PARMS];
  HDC hDC;
  COLORREF color;

  if ((argc < 4) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bAbbrev = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
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
    printf("parms[WIDTH]  = %d\n",parms[WIDTH]);
  }

  for (m = 0; m < 5; m++) {
    color = GetPixel(hDC,parms[LEFT] + m * parms[WIDTH],parms[TOP]);

    if (bDebug)
      printf("color = %x\n",color);

    for (n = 0; n < NUM_SUITS; n++) {
      if (color == suit_colors[n])
        break;
    }

    if (n == NUM_SUITS)
      break;

    if (m)
      putchar(' ');

    if (bAbbrev)
      printf("%c",suit_abbrevs[n]);
    else
      printf("%s",suits[n]);
  }

  if (m)
    putchar(0x0a);

  return 0;
}
