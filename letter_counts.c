#include <stdio.h>
#include <string.h>

#define NUM_LETTERS 26
static int letter_counts[NUM_LETTERS];

#define NUM_LETTER_ROWS 2
#define LETTERS_PER_ROW (NUM_LETTERS / NUM_LETTER_ROWS)

static char usage[] = "usage: letter_counts string (string ...)\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int len;
  int chara;
  int total;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (curr_arg > 1) {
      for (m = 0; m < NUM_LETTERS; m++)
        letter_counts[m] = 0;
    }

    len = strlen(argv[curr_arg]);

    for (m = 0; m < len; m++) {
      chara = argv[curr_arg][m];

      if ((chara >= 'A') && (chara <= 'Z'))
        chara -= 'A';
      else if ((chara >= 'a') && (chara <= 'z'))
        chara -= 'a';
      else
        continue;

      letter_counts[chara]++;
    }

    total = 0;

    for (n = 0; n < NUM_LETTERS; n++)
      total += letter_counts[n];

    for (n = 0; n < NUM_LETTER_ROWS; n++) {
      for (m = 0; m < LETTERS_PER_ROW; m++) {
        printf("   %c",'a' + (n * LETTERS_PER_ROW) + m);

        if (m < LETTERS_PER_ROW - 1)
          putchar(' ');
        else
          putchar(0x0a);
      }

      for (m = 0; m < LETTERS_PER_ROW; m++) {
        printf("%4d",letter_counts[(n * LETTERS_PER_ROW) + m]);

        if (m < LETTERS_PER_ROW - 1)
          putchar(' ');
        else {
          if (!n)
            putchar(0x0a);
          else
            printf(" %4d\n",total);
        }
      }
    }

    if ((argc > 2) && (curr_arg < argc - 1))
      putchar(0x0a);
  }

  return 0;
}
