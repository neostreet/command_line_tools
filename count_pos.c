#include <stdio.h>
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
"usage: count_pos (-verbose) (-exact_countcount) (-on_last) (-percent)\n"
"  (-only_zero) (-sum_ixs) (-is_exact_countcount) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int exact_count;
  bool bOnLast;
  bool bPos;
  bool bPercent;
  bool bOnlyZero;
  bool bSumIxs;
  bool bIsExactCount;
  FILE *fptr;
  int line_len;
  int line_no;
  int count_pos;
  int work;
  double dwork;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  exact_count = -1;
  bOnLast = false;
  bPercent = false;
  bOnlyZero = false;
  bSumIxs = false;
  bIsExactCount = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-exact_count",12))
      sscanf(&argv[curr_arg][12],"%d",&exact_count);
    else if (!strcmp(argv[curr_arg],"-on_last"))
      bOnLast = true;
    else if (!strcmp(argv[curr_arg],"-percent"))
      bPercent = true;
    else if (!strcmp(argv[curr_arg],"-only_zero"))
      bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-sum_ixs"))
      bSumIxs = true;
    else if (!strncmp(argv[curr_arg],"-is_exact_count",15)) {
      sscanf(&argv[curr_arg][15],"%d",&exact_count);
      bIsExactCount = true;
    }
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

  if (bVerbose)
    getcwd(save_dir,_MAX_PATH);

  count_pos = 0;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;
    sscanf(line,"%d",&work);

    if (work > 0) {
      if (!bSumIxs)
        count_pos++;
      else
        count_pos += line_no;

      bPos = true;
    }
    else
      bPos = false;

    line_no++;
  }

  fclose(fptr);

  if (bIsExactCount) {
    if (count_pos == exact_count) {
      if (!bVerbose)
        printf("1\n");
      else
        printf("1 (%d) %s\n",line_no,save_dir);
    }
    else {
      if (!bVerbose)
        printf("0\n");
      else
        printf("0 (%d) %s\n",line_no,save_dir);
    }
  }
  else if ((exact_count == -1) || (count_pos == exact_count)) {
    if (!bOnLast || bPos) {
      if (!bOnlyZero || (count_pos == 0)) {
        if (!bVerbose)
          printf("%d\n",count_pos);
        else {
          if (!bPercent)
            printf("%d (%d) %s\n",count_pos,line_no,save_dir);
          else {
            dwork = (double)count_pos / (double)line_no;
            printf("%lf (%d %d) %s\n",dwork,count_pos,line_no,save_dir);
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
