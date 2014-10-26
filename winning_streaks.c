#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

static char usage[] = "usage: winning_streaks (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char fmt[] = "%s\n";
static char fmt2[] = "%2d %s\n";
static char fmt3[] = "%2d %s %d\n";

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
  int curr_winning_streak_start_line;
  int delta;

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
            printf(fmt2,curr_winning_streak,save_line);
          else
            printf(fmt3,curr_winning_streak,argv[curr_arg],curr_winning_streak_start_line);
        }
      }

      curr_winning_streak = 0;
    }
    else {
      curr_winning_streak++;

      if (curr_winning_streak == 1) {
        strcpy(save_line,line);
        curr_winning_streak_start_line = line_no;
      }

      if (bVerbose) {
        if (curr_winning_streak == 2)
          printf(fmt,save_line);

        if (curr_winning_streak >= 2)
          printf(fmt,line);
      }
    }
  }

  fclose(fptr);

  if (curr_winning_streak >= 2) {
    if (!bVerbose) {
      if (!bDebug)
        printf(fmt2,curr_winning_streak,save_line);
      else
        printf(fmt3,curr_winning_streak,argv[curr_arg],curr_winning_streak_start_line);
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
