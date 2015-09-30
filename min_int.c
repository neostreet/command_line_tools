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
"usage: min_int (-verbose) (-offsetoffset) (-lastn) (-didnt_hit_felt)\n"
"  (-min_less_than_zero) (-losing_session) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int offset;
  int lastn;
  bool bDidntHitFelt;
  bool bMinLessThanZero;
  bool bLosingSession;
  FILE *fptr;
  int linelen;
  int line_no;
  int delta;
  int session_balance;
  int ending_balance;
  int min;
  int min_ix;

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  offset = 0;
  lastn = 0;
  bDidntHitFelt = false;
  bMinLessThanZero = false;
  bLosingSession = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose")) {
      bVerbose = true;
      getcwd(save_dir,_MAX_PATH);
    }
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strncmp(argv[curr_arg],"-last",5))
      sscanf(&argv[curr_arg][5],"%d",&lastn);
    else if (!strcmp(argv[curr_arg],"-didnt_hit_felt"))
      bDidntHitFelt = true;
    else if (!strcmp(argv[curr_arg],"-min_less_than_zero"))
      bMinLessThanZero = true;
    else if (!strcmp(argv[curr_arg],"-losing_session"))
      bLosingSession = true;
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
  min = 0;
  session_balance = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (!bDidntHitFelt)
      sscanf(&line[offset],"%d",&delta);
    else
      sscanf(&line[offset],"%d %d",&delta,&ending_balance);

    session_balance += delta;

    if ((line_no == 1) || (delta < min)) {
      min = delta;
      min_ix = line_no;
    }
  }

  fclose(fptr);

  if (!bMinLessThanZero || (min < 0)) {
    if (!lastn || (line_no - min_ix + 1 <= lastn)) {
      if (!bDidntHitFelt || (ending_balance > 0)) {
        if (!bLosingSession || (session_balance < 0)) {
          if (!bVerbose)
            printf("%d\n",min);
          else
            printf("%d %d %d %s\n",min,min_ix,line_no,save_dir);
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
