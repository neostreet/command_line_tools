#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: winning_streaks (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char malloc_failed[] = "malloc of %d ints failed\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int num_sessions;
  int *deltas;
  int max_winning_streak;
  int max_winning_streak_start;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
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

  num_sessions = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    num_sessions++;
  }

  if ((deltas = (int *)malloc(
    num_sessions * sizeof (int))) == NULL) {
    printf(malloc_failed,num_sessions);
    fclose(fptr);
    return 4;
  }

  fseek(fptr,0L,SEEK_SET);

  for (n = 0; n < num_sessions; n++) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&deltas[n]);
  }

  fclose(fptr);

  max_winning_streak = 0;

  for (n = 0; n < num_sessions - 1; n++) {
    if ((deltas[n] > 0) && (deltas[n + 1] > 0)) {
      for (m = n + 2; m < num_sessions; m++) {
        if (deltas[m] < 0)
          break;
      }
      if (!bVerbose)
        printf("%3d %3d\n",n,m-1);
      else
        printf("%3d %3d (%2d)\n",n,m-1,m-n);

      if (m - n > max_winning_streak) {
        max_winning_streak = m - n;
        max_winning_streak_start = n;
      }

      if (m >= num_sessions - 1)
        break;
      else
        n = m - 1;
    }
  }

  free(deltas);

  if (bVerbose) {
    printf("\nmax_winning_streak = %2d (%3d)\n",
      max_winning_streak,max_winning_streak_start);
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
