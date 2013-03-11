#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: addf (-verbose) (-offsetoffset) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int offset;
  FILE *fptr;
  int linelen;
  int line_no;
  double work;
  double total;
  double negative_total;
  double positive_total;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  line_no = 0;
  total = (double)0;

  if (bVerbose) {
    negative_total = (double)0;
    positive_total = (double)0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%lf",&work);

    total += work;

    if (bVerbose) {
      if (work < (double)0)
        negative_total += work;
      else
        positive_total += work;
    }
  }

  fclose(fptr);

  if (!bVerbose)
    printf("%lf\n",total);
  else
    printf("%lf %lf %lf\n",total,negative_total,positive_total);

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
