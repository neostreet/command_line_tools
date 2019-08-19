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
char line[MAX_LINE_LEN];

static char usage[] =
"usage: peak_ix (-debug) (-terse) (-verbose) (-streak_first) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bTerse;
  bool bVerbose;
  bool bStreakFirst;
  FILE *fptr;
  int linelen;
  int line_no;
  int work;
  int total;
  int peak;
  int peak_ix;
  int curr_plus_streak;
  int peak_plus_streak;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bTerse = false;
  bVerbose = false;
  bStreakFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-streak_first"))
      bStreakFirst = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 3;
  }

  getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  line_no = 0;

  total = 0;
  curr_plus_streak = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    sscanf(&line[0],"%d",&work);

    total += work;

    if (work <= 0)
      curr_plus_streak = 0;
    else
      curr_plus_streak++;

    if ((line_no == 1) || (total > peak)) {
      peak = total;
      peak_ix = line_no - 1;
      peak_plus_streak = curr_plus_streak;
    }
  }

  fclose(fptr);

  if (!bVerbose) {
    if (!bStreakFirst)
      printf("%d %d %d\n",peak_ix,peak,peak_plus_streak);
    else
      printf("%d %d %d\n",peak_plus_streak,peak_ix,peak);
  }
  else {
    if (!bStreakFirst)
      printf("%d %d %d %s\n",peak_ix,peak,peak_plus_streak,save_dir);
    else
      printf("%d %d %d %s\n",peak_plus_streak,peak_ix,peak,save_dir);
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
