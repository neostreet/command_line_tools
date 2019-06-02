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
"usage: max_freefall (-debug) (-terse) (-verbose)\n"
"  (-pct_first) (-len_first) (-by_len) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bTerse;
  bool bVerbose;
  bool bPctFirst;
  bool bLenFirst;
  bool bByLen;
  FILE *fptr;
  int linelen;
  int line_no;
  int work;
  int curr_freefall;
  int curr_freefall_start_ix;
  int curr_freefall_end_ix;
  int curr_freefall_len;
  int new_max;
  int max_freefall;
  int max_freefall_start_ix;
  int max_freefall_end_ix;
  int max_freefall_len;
  double dwork;

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bTerse = false;
  bVerbose = false;
  bPctFirst = false;
  bLenFirst = false;
  bByLen = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-pct_first"))
      bPctFirst = true;
    else if (!strcmp(argv[curr_arg],"-len_first"))
      bLenFirst = true;
    else if (!strcmp(argv[curr_arg],"-by_len"))
      bByLen = true;
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

  if (bPctFirst && bLenFirst) {
    printf("can't specify both -pct_first and -len_first\n");
    return 4;
  }

  getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  line_no = 0;
  curr_freefall = -1;
  max_freefall = -1;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    sscanf(&line[0],"%d",&work);

    if (work > 0) {
      if (curr_freefall != -1) {
        curr_freefall_end_ix = line_no;
        curr_freefall_len = curr_freefall_end_ix - curr_freefall_start_ix;

        new_max = 0;

        if (max_freefall == -1)
          new_max = 1;
        else {
          if (!bByLen) {
            if (curr_freefall > max_freefall)
              new_max = 1;
          }
          else {
            if (curr_freefall_len > max_freefall_len)
              new_max = 1;
          }
        }

        if (new_max) {
          max_freefall = curr_freefall;
          max_freefall_start_ix = curr_freefall_start_ix;
          max_freefall_end_ix = curr_freefall_end_ix;
          max_freefall_len = curr_freefall_len;
        }
      }

      curr_freefall = -1;
    }
    else {
      if (curr_freefall != -1)
        curr_freefall += (work * -1);
      else {
        curr_freefall = work * -1;
        curr_freefall_start_ix = line_no;
      }
    }
  }

  fclose(fptr);

  if (max_freefall != -1) {
    if (!bVerbose) {
      if (!bLenFirst)
        printf("%d %d\n",max_freefall,max_freefall_len);
      else
        printf("%d %d\n",max_freefall_len,max_freefall);
    }
    else {
      if (!bLenFirst) {
        printf("%d %d (%d %d %d) %s\n",max_freefall,max_freefall_len,
          max_freefall_start_ix,max_freefall_end_ix - 1,line_no,save_dir);
      }
      else {
        printf("%d (%d %d %d) %d %s\n",max_freefall_len,
          max_freefall_start_ix,max_freefall_end_ix - 1,line_no,max_freefall,save_dir);
      }
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
