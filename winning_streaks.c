#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

static char usage[] = "usage: winning_streaks (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char fmt[] = "%2d %d\n";
static char fmt2[] = "%2d %10d %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  FILE *fptr;
  int line_len;
  int line_no;
  int curr_winning_streak;
  int curr_moving_sum;
  int curr_winning_streak_end_line;
  int delta;
  int first_delta;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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
  curr_winning_streak = 0;
  curr_moving_sum = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&delta);

    if (delta <= 0) {
      if (curr_winning_streak >= 2) {
        if (!bVerbose) {
          if (!bDebug)
            printf(fmt,curr_winning_streak,curr_winning_streak_end_line);
          else
            printf(fmt2,curr_winning_streak,curr_moving_sum,save_line);
        }
      }

      curr_winning_streak = 0;
      curr_moving_sum = 0;
    }
    else {
      curr_winning_streak++;

      if (curr_winning_streak == 1)
        first_delta = delta;

      curr_moving_sum += delta;

      if (bVerbose) {
        if (curr_winning_streak == 2)
          printf(fmt2,1,first_delta,save_line);

        if (curr_winning_streak >= 2)
          printf(fmt2,curr_winning_streak,curr_moving_sum,line);
      }

      if (curr_winning_streak) {
        strcpy(save_line,line);
        curr_winning_streak_end_line = line_no;
      }
    }
  }

  fclose(fptr);

  if (curr_winning_streak >= 2) {
    if (!bVerbose) {
      if (!bDebug)
        printf(fmt,curr_winning_streak,curr_winning_streak_end_line);
      else
        printf(fmt2,curr_winning_streak,curr_moving_sum,save_line);
    }
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
