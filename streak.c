#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: streak (-debug) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  int bDebug;
  FILE *fptr;
  int line_len;
  int line_no;
  int plus_streak;
  int minus_streak;
  int max_plus_streak;
  int max_minus_streak;
  int work;
  int chara;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  line_no = 0;
  plus_streak = 0;
  minus_streak = 0;
  max_plus_streak = 0;
  max_minus_streak = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (!line_len)
      continue;

    chara = line[0];

    if (((chara >= 'a') && (chara <= 'z')) ||
        ((chara >= 'A') && (chara <= 'Z')))
      continue;

    line_no++;

    sscanf(line,"%d",&work);

    if (bDebug)
      printf("debug: %10d\n",work);

    if (line_no == 1) {
      if (work <= 0)
        minus_streak = 1;
      else
        plus_streak = 1;
    }
    else {
      if (work <= 0) {
        if (minus_streak)
          minus_streak++;
        else {
          printf("+%d\n",plus_streak);

          if (plus_streak > max_plus_streak)
            max_plus_streak = plus_streak;

          plus_streak = 0;
          minus_streak = 1;
        }
      }
      else {
        if (plus_streak)
          plus_streak++;
        else {
          printf("-%d\n",minus_streak);

          if (minus_streak > max_minus_streak)
            max_minus_streak = minus_streak;

          minus_streak = 0;
          plus_streak = 1;
        }
      }
    }
  }

  if (plus_streak >= 1) {
    printf("+%d\n",plus_streak);

    if (plus_streak > max_plus_streak)
      max_plus_streak = plus_streak;
  }
  else if (minus_streak >= 1) {
    printf("-%d\n",minus_streak);

    if (minus_streak > max_minus_streak)
      max_minus_streak = minus_streak;
  }

  fclose(fptr);

  putchar(0x0a);

  printf("nobs:             %6d\n",line_no);
  printf("max_plus_streak:  %6d\n",max_plus_streak);
  printf("max_minus_streak: %6d\n",max_minus_streak);

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