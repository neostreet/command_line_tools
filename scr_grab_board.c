#include <windows.h>
#include <stdio.h>
#include <string.h>

struct left_top {
  int left;
  int top;
};

#define NUM_LEFT_TOPS 3
static struct left_top left_tops[NUM_LEFT_TOPS];

static char usage[] =
"usage: scr_grab_board (-debug) (-abbrev) (-skip_suit)\n"
"  num_cards left1 top1 left2 top2 left3 top3 width\n";

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
  bool bSkipSuit;
  int num_cards;
  int width;
  HDC hDC;
  COLORREF colors[NUM_LEFT_TOPS];

  if ((argc < 9) || (argc > 12)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bAbbrev = false;
  bSkipSuit = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-skip_suit"))
      bSkipSuit = true;
    else
      break;
  }

  if (argc - curr_arg != 8) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg++],"%d",&num_cards);

  if ((num_cards < 1) || (num_cards > 5)) {
    printf("invalid value %d for num_cards; must be >= 1 and <= 5\n",num_cards);
    return 3;
  }

  hDC = GetDC(NULL);

  if (!hDC) {
    printf("GetDC failed for screen\n");
    return 4;
  }

  for (n = 0; n < NUM_LEFT_TOPS; n++) {
    sscanf(argv[curr_arg + n*2],"%d",&left_tops[n].left);
    sscanf(argv[curr_arg + n*2 + 1],"%d",&left_tops[n].top);
  }

  sscanf(argv[argc-1],"%d",&width);

  for (m = 0; m < num_cards; m++) {
    for (n = 0; n < NUM_LEFT_TOPS; n++) {
      colors[n] = GetPixel(hDC,left_tops[n].left + m * width,left_tops[n].top);

      if (bDebug)
        printf("colors[%d] = %x\n",n,colors[n]);
    }

    for (n = 0; n < NUM_DENOMS * 2; n++) {
      if ((denom_colors[n][0] == colors[0]) && (denom_colors[n][1] == colors[1]))
        break;
    }

    if (n == NUM_DENOMS * 2)
      break;

    if (m)
      putchar(' ');

    n = n % NUM_DENOMS;

    if (bAbbrev)
      printf("%c",denom_abbrevs[n]);
    else
      printf("%s",denoms[n]);

    if (!bSkipSuit) {
      for (n = 0; n < NUM_SUITS; n++) {
        if (colors[2] == suit_colors[n])
          break;
      }

      if (n == NUM_SUITS)
        printf("?");
      else if (bAbbrev)
        printf("%c",suit_abbrevs[n]);
      else
        printf("%s",suits[n]);
    }
  }

  if (m)
    putchar(0x0a);

  return 0;
}
