#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define NUM_LETTERS 26
static int letter_counts[NUM_LETTERS];

#define NUM_LETTER_ROWS 2
#define LETTERS_PER_ROW (NUM_LETTERS / NUM_LETTER_ROWS)

static char usage[] = "usage: fletter_counts filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  FILE *fptr;
  int line_len;
  int line_no;
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

    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      len = strlen(line);

      for (m = 0; m < len; m++) {
        chara = line[m];

        if ((chara >= 'A') && (chara <= 'Z'))
          chara -= 'A';
        else if ((chara >= 'a') && (chara <= 'z'))
          chara -= 'a';
        else
          continue;

        letter_counts[chara]++;
      }
    }

    fclose(fptr);

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

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}
