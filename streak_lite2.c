#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define DATE_LEN 11
static char date[DATE_LEN+1];
static char save_date[DATE_LEN+1];

static char usage[] = "usage: streak_lite2 (-sum_first) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int parse_line(char *line,int line_len,int *delta,char *date);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bSumFirst;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  int delta;
  int positive_streak;
  int negative_streak;
  int sum_delta;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bSumFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sum_first"))
      bSumFirst = true;
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
  date[DATE_LEN] = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = parse_line(line,line_len,&delta,date);

    if (retval) {
      printf("parse_line() failed on line %d: %d\n",line_no,retval);
      return 4;
    }

    if (line_no == 1) {
      if (delta < 0) {
        negative_streak = 1;
        positive_streak = 0;
      }
      else {
        positive_streak = 1;
        negative_streak = 0;
      }

      sum_delta = delta;
    }
    else {
      if (!delta) {
        // a delta of 0 extends the current streak
        if (negative_streak)
          negative_streak++;
        else
          positive_streak++;

        sum_delta += delta;
      }
      else if (delta < 0) {
        if (negative_streak) {
          negative_streak++;
          sum_delta += delta;
        }
        else {
          if (!bSumFirst)
            printf("+%2d %7d %s\n",positive_streak,sum_delta,save_date);
          else
            printf("%d %d %s\n",sum_delta,positive_streak,save_date);

          negative_streak = 1;
          positive_streak = 0;
          sum_delta = delta;
        }
      }
      else {
        if (positive_streak) {
          positive_streak++;
          sum_delta += delta;
        }
        else {
          if (!bSumFirst)
            printf("-%2d %7d %s\n",negative_streak,sum_delta,save_date);
          else
            printf("%d %d %s\n",sum_delta,negative_streak,save_date);

          positive_streak = 1;
          negative_streak = 0;
          sum_delta = delta;
        }
      }
    }

    strcpy(save_date,date);
  }

  if (positive_streak) {
    if (!bSumFirst)
      printf("+%2d %7d %s\n",positive_streak,sum_delta,save_date);
    else
      printf("%d %d %s\n",sum_delta,positive_streak,save_date);
  }
  else {
    if (!bSumFirst)
      printf("-%2d %7d %s\n",negative_streak,sum_delta,save_date);
    else
      printf("%d %d %s\n",sum_delta,negative_streak,save_date);
  }

  fclose(fptr);

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

static int parse_line(char *line,int line_len,int *delta,char *date)
{
  int m;
  int n;

  sscanf(line,"%d",delta);

  for (n = line_len - 1; n >= 0; n--) {
    if (line[n] == '\t')
      break;
  }

  if (n < 0)
    return 1;

  n++;

  if (line_len - n != DATE_LEN)
    return 2;

  for (m = 0; m < DATE_LEN; m++)
    date[m] = line[n+m];

  return 0;
}
