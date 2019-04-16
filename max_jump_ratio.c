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
"usage: max_jump_ratio (-debug) (-verbose) (-offsetoffset)\n"
"  (-above_water) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int offset;
  bool bAboveWater;
  FILE *fptr;
  int linelen;
  int line_no;
  int work;
  int prev;
  double jump_ratio;
  double max_jump_ratio;
  int max_num;
  int max_denom;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bAboveWater = false;
  offset = 0;
  prev = 0;
  max_jump_ratio = (double)0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strcmp(argv[curr_arg],"-above_water"))
      bAboveWater = true;
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

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%d",&work);

    if (line_no > 1) {
      if (prev) {
        if (!bAboveWater || ((work > 0) && (prev > 0))) {
          jump_ratio = (double)work / (double)prev;

          if (jump_ratio > max_jump_ratio) {
            max_jump_ratio = jump_ratio;

            if (bVerbose) {
              max_num = work;
              max_denom = prev;
            }
          }
        }
      }
    }

    prev = work;
  }

  fclose(fptr);

  if (!bVerbose)
    printf("%lf\n",max_jump_ratio);
  else
    printf("%lf (%d %d) %s\n",max_jump_ratio,max_num,max_denom,save_dir);

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
