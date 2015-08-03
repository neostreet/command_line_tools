#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

static char usage[] = "usage: nonzero_streaks (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int curr_nonzero_streak;
  int curr_nonzero_streak_start_ix;

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

  line_no = 0;
  curr_nonzero_streak = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&val);

    line_no++;

    if (!val) {
      if (curr_nonzero_streak > 0) {
        if (!bVerbose)
          printf("%d (%d)\n",curr_nonzero_streak,curr_nonzero_streak_start_ix);
        else
          printf("%d (%d) %s\n",curr_nonzero_streak,curr_nonzero_streak_start_ix,save_line);
      }

      curr_nonzero_streak = 0;
    }
    else {
      if (!curr_nonzero_streak) {
        curr_nonzero_streak_start_ix = line_no;

        if (bVerbose)
          strcpy(save_line,line);
      }

      curr_nonzero_streak++;
    }
  }

  if (curr_nonzero_streak > 0) {
    if (!bVerbose)
      printf("%d (%d)\n",curr_nonzero_streak,curr_nonzero_streak_start_ix);
    else
      printf("%d (%d) %s\n",curr_nonzero_streak,curr_nonzero_streak_start_ix,save_line);
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
