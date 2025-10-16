#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: run_roi (-verbose) (-terse) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bTerse;
  FILE *fptr;
  int line_len;
  int delta;
  int wagered;
  int total_delta;
  int total_wagered;
  double work;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bTerse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bVerbose && bTerse) {
    printf("can't specify both -verbose and -terse\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  total_delta = 0;
  total_wagered = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d %d",&delta,&wagered);

    if (!wagered)
      continue;

    total_delta += delta;
    total_wagered += wagered;

    work = (double)total_delta / (double)total_wagered;

    if (!bVerbose)
      printf("%lf %d %d\n",work,total_delta,total_wagered);
    else
      printf("%lf %d %D %s\n",work,total_delta,total_wagered,line);
  }

  fclose(fptr);

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
