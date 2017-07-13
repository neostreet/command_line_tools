#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[2][MAX_LINE_LEN];

static char usage[] = "usage: div2files (-verbose) filename filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  FILE *fptr[2];
  int linelen[2];
  int line_no;
  int val[2];
  double quotient;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  for (n = 0; n < 2; n++) {
    if ((fptr[n] = fopen(argv[curr_arg+n],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg+n]);
      return 3;
    }
  }

  line_no = 0;

  for ( ; ; ) {
    for (n = 0; n < 2; n++) {
      GetLine(fptr[n],line[n],&linelen[n],MAX_LINE_LEN);

      if (feof(fptr[n]))
        break;

      if (n)
        line_no++;

      sscanf(line[n],"%d",&val[n]);
    }

    if (n < 2)
      break;

    if (val[1])
      quotient = (double)val[0] / (double)val[1];
    else
      quotient = (double)-1;

    if (!bVerbose)
      printf("%lf\n",quotient);
    else
      printf("%lf (%d) %s\n",quotient,val[0],line[1]);
  }

  for (n = 0; n < 2; n++)
    fclose(fptr[n]);

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
