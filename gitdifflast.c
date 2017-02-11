#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPARE_NUM 2

#define MAX_COMMIT_NAME_LEN 2048
static char commits[COMPARE_NUM][MAX_COMMIT_NAME_LEN];

#define MAX_COMMAND_LINE_LEN 4096
static char command_line[MAX_COMMAND_LINE_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: gitdifflast (-debug) (-backn) (-difftool) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char temp_log[] = "tem.log";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int back;
  bool bDifftool;
  int n;
  int len;
  FILE *fptr;
  int line_len;
  int line_no;
  int commit_num;
  int commit_count;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  back = 1;
  bDifftool = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strncmp(argv[curr_arg],"-back",5))
      sscanf(&argv[curr_arg][5],"%d",&back);
    else if (!strcmp(argv[curr_arg],"-difftool"))
      bDifftool = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sprintf(command_line,"git log %s > %s",argv[curr_arg],temp_log);
  system(command_line);

  if ((fptr = fopen(temp_log,"r")) == NULL) {
    printf(couldnt_open,temp_log);
    return 3;
  }

  line_no = 0;
  commit_num = 1 + back;
  commit_count = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (!strncmp(line,"commit ",7)) {
      if (!commit_count)
        strcpy(&commits[0][0],&line[7]);
      else if (commit_count == back)
        strcpy(&commits[1][0],&line[7]);

      commit_count++;

      if (commit_count == commit_num)
        break;
    }
  }

  fclose(fptr);
  remove(temp_log);

  if (commit_count != commit_num) {
    printf("couldn't find two commits\n");
    return 4;
  }

#ifdef WNT
  if (bDifftool)
    sprintf(command_line,"git difftool %s %s %s",commits[1],commits[0],argv[curr_arg]);
  else
#endif
    sprintf(command_line,"git diff %s %s %s",commits[1],commits[0],argv[curr_arg]);

  if (bDebug)
    printf("%s\n",command_line);
  else
    system(command_line);

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
