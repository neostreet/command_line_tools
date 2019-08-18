#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: max_plus_streak (-verbose) filename\n";
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
  int max_plus_streak;
  int max_plus_streak_start_ix;
  int curr_plus_streak;
  int curr_plus_streak_start_ix;

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

  if (bVerbose)
    getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  line_no = 0;
  max_plus_streak = 0;
  curr_plus_streak = 0;


  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&val);

    if (val <= 0) {
      if (curr_plus_streak > max_plus_streak) {
        max_plus_streak = curr_plus_streak;
        max_plus_streak_start_ix = curr_plus_streak_start_ix;
      }

      curr_plus_streak = 0;
    }
    else {
      if (!curr_plus_streak)
        curr_plus_streak_start_ix = line_no;

      curr_plus_streak++;
    }

    line_no++;
  }

  if (curr_plus_streak > max_plus_streak) {
    max_plus_streak = curr_plus_streak;
    max_plus_streak_start_ix = curr_plus_streak_start_ix;
  }

  fclose(fptr);

  if (!bVerbose)
    printf("%d (%d)\n",max_plus_streak,max_plus_streak_start_ix+1);
  else
    printf("%d (%d) %s\n",max_plus_streak,max_plus_streak_start_ix+1,save_dir);

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
