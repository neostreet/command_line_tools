#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: nonzero_streaks filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int curr_nonzero_streak;
  int curr_nonzero_streak_start_ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
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
      if (curr_nonzero_streak > 0)
        printf("%d (%d)\n",curr_nonzero_streak,curr_nonzero_streak_start_ix);

      curr_nonzero_streak = 0;
    }
    else {
      if (!curr_nonzero_streak)
        curr_nonzero_streak_start_ix = line_no;

      curr_nonzero_streak++;
    }
  }

  if (curr_nonzero_streak > 0)
    printf("%d (%d)\n",curr_nonzero_streak,curr_nonzero_streak_start_ix);

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
