#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: max_nonzero_streak filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int max_nonzero_streak;
  int max_nonzero_streak_start_ix;
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
  max_nonzero_streak = 0;
  curr_nonzero_streak = 0;


  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&val);

    if (!val) {
      if (curr_nonzero_streak > max_nonzero_streak) {
        max_nonzero_streak = curr_nonzero_streak;
        max_nonzero_streak_start_ix = curr_nonzero_streak_start_ix;
      }

      curr_nonzero_streak = 0;
    }
    else {
      if (!curr_nonzero_streak)
        curr_nonzero_streak_start_ix = line_no;

      curr_nonzero_streak++;
    }

    line_no++;
  }

  if (curr_nonzero_streak > max_nonzero_streak) {
    max_nonzero_streak = curr_nonzero_streak;
    max_nonzero_streak_start_ix = curr_nonzero_streak_start_ix;
  }

  fclose(fptr);

  printf("%d (%d)\n",max_nonzero_streak,max_nonzero_streak_start_ix+1);

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
