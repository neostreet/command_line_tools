#include <windows.h>
#include <stdio.h>
#include <string.h>

struct left_top {
  int left;
  int top;
};

#define NUM_LEFT_TOPS 2
static struct left_top left_tops[NUM_LEFT_TOPS];

static char usage[] =
"usage: scr_grab_denom (-debug) (-abbrev) left1 top1 left2 top2\n";

static char *denoms[] = {
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "jack",
  "queen",
  "king",
  "ace"
};
#define NUM_DENOMS (sizeof denoms / sizeof (char *))

static char *denom_abbrevs = "23456789TJQKA";

static int denom_colors[][2] = {
  { 0x10101, 0x0 },
  { 0x5f5f5f, 0xa0a0a },
  { 0xffffff, 0x9d9d9d },
  { 0x70707, 0x70707 },
  { 0x2e2e2e, 0x40404 },
  { 0xffffff, 0x0 },
  { 0x636363, 0xa0a0a },
  { 0xb0b0b, 0x40404 },
  { 0x0, 0x40404 },
  { 0x282828, 0xffffff },
  { 0x626262, 0x50505 },
  { 0x0, 0xffffff },
  { 0x0, 0x0 },
  { 0x1111ca, 0x1010ca },
  { 0x6969dd, 0x1a1acc },
  { 0xffffff, 0xa3a3ea },
  { 0x1717cb, 0x1717cb },
  { 0x3b3bd3, 0x1414cb },
  { 0xffffff, 0x1010ca },
  { 0x6d6dde, 0x1a1acc },
  { 0x1b1bcc, 0x1414cb },
  { 0x1010ca, 0x1414cb },
  { 0x3636d2, 0xffffff },
  { 0x6c6cde, 0x1515cb },
  { 0x1010ca, 0xffffff },
  { 0x1010ca, 0x1010ca }
};

static char couldnt_open[] = "couldn't open %s\n";
static char fmt[] = "%08x\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bAbbrev;
  HDC hDC;
  COLORREF colors[2];

  if ((argc < 5) || (argc > 7)) {
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

  if (argc - curr_arg != 4) {
    printf(usage);
    return 2;
  }

  hDC = GetDC(NULL);

  if (!hDC) {
    printf("GetDC failed for screen\n");
    return 3;
  }

  for (n = 0; n < 2; n++) {
    sscanf(argv[curr_arg + n*2],"%d",&left_tops[n].left);
    sscanf(argv[curr_arg + n*2 + 1],"%d",&left_tops[n].top);
    colors[n] = GetPixel(hDC,left_tops[n].left,left_tops[n].top);

    if (bDebug)
      printf("colors[%d] = %x\n",n,colors[n]);
  }

  for (n = 0; n < NUM_DENOMS * 2; n++) {
    if ((denom_colors[n][0] == colors[0]) && (denom_colors[n][1] == colors[1]))
      break;
  }

  if (n == NUM_DENOMS * 2) {
    printf("couldn't determine denom\n");

    return 4;
  }

  n = n % NUM_DENOMS;

  if (bAbbrev)
    printf("%c\n",denom_abbrevs[n]);
  else
    printf("%s\n",denoms[n]);

  return 0;
}
