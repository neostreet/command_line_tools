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
"usage: count_pos (-debug) (-exact_countcount) (-on_last) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int exact_count;
  bool bOnLast;
  bool bPos;
  FILE *fptr;
  int line_len;
  int line_no;
  int count_pos;
  int work;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  exact_count = -1;
  bOnLast = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strncmp(argv[curr_arg],"-exact_count",12))
      sscanf(&argv[curr_arg][12],"%d",&exact_count);
    else if (!strcmp(argv[curr_arg],"-on_last"))
      bOnLast = true;
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

  if (bDebug)
    getcwd(save_dir,_MAX_PATH);

  count_pos = 0;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    sscanf(line,"%d",&work);

    if (work > 0) {
      count_pos++;
      bPos = true;
    }
    else
      bPos = false;
  }

  fclose(fptr);

  if ((exact_count == -1) || (count_pos == exact_count)) {
    if (!bOnLast || bPos) {
      if (!bDebug)
        printf("%d\n",count_pos);
      else
        printf("%d %s\n",count_pos,save_dir);
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
