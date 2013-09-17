#include <stdio.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif
#include <string.h>

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: runtot_minmax_int (-debug) (-verbose) (-offsetoffset) filename\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  int runtot;
  int min;
  int max;
  int work;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  bDebug = false;
  bVerbose = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose")) {
      bVerbose = true;
      getcwd(save_dir,_MAX_PATH);
    }
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(&line[offset],"%d",&work);

    runtot += work;

    if (bDebug)
      printf("%d",runtot);

    if (!line_no) {
      min = runtot;
      max = runtot;

      if (bDebug)
        printf(" min max\n");
    }
    else if (runtot < min) {
      min = runtot;

      if (bDebug)
        printf(" min\n");
    }
    else if (runtot > max) {
      max = runtot;

      if (bDebug)
        printf(" max\n");
    }
    else if (bDebug)
      putchar(0x0a);

    line_no++;
  }

  fclose(fptr);

  if (!bVerbose)
    printf("%10d %10d\n",min,max);
  else
    printf("%10d %10d %s/%s\n",min,max,save_dir,argv[curr_arg]);

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
