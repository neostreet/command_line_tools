#include <stdio.h>
#include <string.h>

static char usage[] =
"usage: run_win_pct (-verbose) (-ltpct) filename\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  double lt_pct;
  FILE *fptr;
  int line_len;
  int nobs;
  int wins;
  int work;
  double win_pct;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  lt_pct = (double)0.0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-lt",3))
      sscanf(&argv[curr_arg][3],"%lf",&lt_pct);
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

  nobs = 0;
  wins = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&work);

    if (feof(fptr))
      break;

    nobs++;

    if (work > 0)
      wins++;

    win_pct = (double)wins / (double)nobs;

    if (win_pct < lt_pct) {
      if (!bVerbose)
        printf("%lf\n",win_pct);
      else
        printf("%lf (%d %d) %s\n",win_pct,wins,nobs,line);
    }
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
