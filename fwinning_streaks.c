#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: fwinning_streaks (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

#define MAX_DELTAS 300
static int deltas[MAX_DELTAS];

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int m;
  int n;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int num_deltas;
  int max_winning_streak;
  int max_winning_streak_start;
  int num_positive_deltas;

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

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    for (n = 0; ; n++) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (n == MAX_DELTAS) {
        printf("MAX_DELTAS value of %d exceeded in %s\n",MAX_DELTAS,filename);
        return 4;
      }

      sscanf(line,"%d",&deltas[n]);
    }

    fclose(fptr);

    num_deltas = n;

    if (bDebug && bVerbose) {
      for (n = 0; n < num_deltas; n++)
        printf("%d\n",deltas[n]);

      printf("---------------------\n");
    }

    max_winning_streak = 0;
    max_winning_streak_start = -1;
    num_positive_deltas = 0;

    for (n = 0; n < num_deltas - 1; n++) {
      if (deltas[n] > 0)
        num_positive_deltas++;

      if ((deltas[n] > 0) && (deltas[n + 1] > 0)) {
        for (m = n + 2; m < num_deltas; m++) {
          if (deltas[m] <= 0)
            break;
        }

        if (bVerbose)
          printf("%3d %3d (%2d)\n",n,m-1,m-n);

        if (m - n > max_winning_streak) {
          max_winning_streak = m - n;
          max_winning_streak_start = n;
        }

        if (m >= num_deltas - 1)
          break;
        else
          n = m - 1;
      }
    }

    if (num_positive_deltas && !max_winning_streak)
      max_winning_streak = 1;

    if (!bVerbose) {
      if (!bDebug)
        printf("%d\n",max_winning_streak);
      else
        printf("%d %s\n",max_winning_streak,filename);
    }
    else {
      printf("\nmax_winning_streak = %2d (%3d)\n",
        max_winning_streak,max_winning_streak_start);
    }
  }

  fclose(fptr0);

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
