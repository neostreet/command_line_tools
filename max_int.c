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
"usage: max_int (-verbose) (-offsetoffset) (-lastn) (-dolphin_leap)\n"
"  (-max_greater_than_zero) (-winning_session) (-running) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int offset;
  int lastn;
  bool bDolphinLeap;
  bool bMaxGreaterThanZero;
  bool bWinningSession;
  bool bRunning;
  FILE *fptr;
  int linelen;
  int line_no;
  int delta;
  int session_balance;
  int runtot;
  int max;
  int max_ix;
  int max_runtot;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  offset = 0;
  lastn = 0;
  bDolphinLeap = false;
  bMaxGreaterThanZero = false;
  bWinningSession = false;
  bRunning = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose")) {
      bVerbose = true;
      getcwd(save_dir,_MAX_PATH);
    }
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strncmp(argv[curr_arg],"-last",5))
      sscanf(&argv[curr_arg][5],"%d",&lastn);
    else if (!strcmp(argv[curr_arg],"-dolphin_leap"))
      bDolphinLeap = true;
    else if (!strcmp(argv[curr_arg],"-max_greater_than_zero"))
      bMaxGreaterThanZero = true;
    else if (!strcmp(argv[curr_arg],"-winning_session"))
      bWinningSession = true;
    else if (!strcmp(argv[curr_arg],"-running"))
      bRunning = true;
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
  max = 0;
  session_balance = 0;

  if (bDolphinLeap)
    runtot = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%d",&delta);

    session_balance += delta;

    if ((line_no == 1) || (delta > max)) {
      max = delta;
      max_ix = line_no;
      max_runtot = runtot;

      if (bRunning) {
        if (!bMaxGreaterThanZero || (max > 0)) {
          if (!lastn || (line_no - max_ix + 1 <= lastn)) {
            if (!bDolphinLeap || ((max_runtot < 0) && (max_runtot + max > 0))) {
              if (!bWinningSession || (session_balance > 0)) {
                if (!bVerbose)
                  printf("%d\n",max);
                else
                  printf("%d %d %d %s\n",max,max_ix,line_no,save_dir);
              }
            }
          }
        }
      }
    }

    if (bDolphinLeap)
      runtot += delta;
  }

  fclose(fptr);

  if (!bRunning) {
    if (!bMaxGreaterThanZero || (max > 0)) {
      if (!lastn || (line_no - max_ix + 1 <= lastn)) {
        if (!bDolphinLeap || ((max_runtot < 0) && (max_runtot + max > 0))) {
          if (!bWinningSession || (session_balance > 0)) {
            if (!bVerbose)
              printf("%d\n",max);
            else
              printf("%d %d %d %s\n",max,max_ix,line_no,save_dir);
          }
        }
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
