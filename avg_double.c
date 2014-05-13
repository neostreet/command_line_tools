#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] =
"usage: avg_double (-verbose) (-runtot) (-abs) filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bRunTot;
  bool bAbs;
  FILE *fptr;
  int linelen;
  int line_no;
  double tot;
  double work;
  double dwork;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bRunTot = false;
  bAbs = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-runtot"))
      bRunTot = true;
    else if (!strcmp(argv[curr_arg],"-abs"))
      bAbs = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    if (bVerbose)
      printf("%s\n",argv[curr_arg]);

    line_no = 0;
    tot = (double)0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;
      sscanf(line,"%lf",&work);

      if (bAbs) {
        if (work < (double)0)
          work *= (double)-1;
      }

      tot += work;

      if (bVerbose) {
        dwork = tot / (double)line_no;

        if (!bRunTot)
          printf("  %lf %lf\n",work,dwork);
        else
          printf("  %lf %lf %lf\n",work,tot,dwork);
      }
    }

    fclose(fptr);

    if (!bVerbose) {
      dwork = tot / (double)line_no;

      if (!bRunTot)
        printf("%s: %lf\n",argv[curr_arg],dwork);
      else
        printf("%s: %lf %lf\n",argv[curr_arg],tot,dwork);
    }
  }

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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
