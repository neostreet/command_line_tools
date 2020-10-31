#include <stdio.h>
#include <string.h>

static char usage[] =
"usage: run_delta_pct (-verbose) (-skip_until_bal_ge_val) start_bal filename\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int start_bal;
  int skip_until_bal_ge_val;
  FILE *fptr;
  int balance;
  int line_len;
  int work;
  double delta_pct;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  skip_until_bal_ge_val = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-skip_until_bal_ge_",19))
      sscanf(&argv[curr_arg][19],"%d",&skip_until_bal_ge_val);
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg+1]);
    return 3;
  }

  sscanf(argv[curr_arg],"%d",&start_bal);
  balance = start_bal;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&work);

    if (feof(fptr))
      break;

    if (skip_until_bal_ge_val != -1) {
      if (balance < skip_until_bal_ge_val) {
        balance += work;
        continue;
      }
      else
        skip_until_bal_ge_val = -1;
    }

    delta_pct = (double)work / (double)balance;

    if (!bVerbose)
      printf("%lf\n",delta_pct);
    else
      printf("%lf %s (%d)\n",delta_pct,line,balance);

    balance += work;
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
