#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: runtot_int7 (-initial_balbal) (-verbose) (-start_bal) (-start_and_end)\n"
"  (-offsetoffset) (-line_numbers) filename\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bStartBal;
  bool bStartAndEnd;
  bool bLineNumbers;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  int runtot[2];
  int work[2];

  if ((argc < 2) || (argc > 12)) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++)
    runtot[n] = 0;

  bVerbose = false;
  bStartBal = false;
  bStartAndEnd = false;
  bLineNumbers = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12))
      sscanf(&argv[curr_arg][12],"%d",&runtot[0]);
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-start_bal"))
      bStartBal = true;
    else if (!strcmp(argv[curr_arg],"-start_and_end"))
      bStartAndEnd = true;
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strcmp(argv[curr_arg],"-line_numbers"))
      bLineNumbers = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bStartBal && bStartAndEnd) {
    printf("can't specify both -start_bal and -start_and_end\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 7;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%d %d",&work[0],&work[1]);

    if (!bStartBal && !bStartAndEnd) {
      for (n = 0; n < 2; n++)
        runtot[n] += work[n];
    }

    if (!bVerbose)
      printf("%d %d",runtot[0],runtot[1]);
    else
      printf("%10d %10d %10d %s",runtot[0],runtot[1],runtot[1] - runtot[0],line);

    if (!bLineNumbers)
      putchar(0x0a);
    else
      printf(" %d\n",line_no);

    if (bStartBal || bStartAndEnd) {
      for (n = 0; n < 2; n++)
        runtot[n] += work[n];
    }
  }

  fclose(fptr);

  if (bStartAndEnd) {
    if (!bVerbose)
      printf("%d %d",runtot[0],runtot[1]);

    if (!bLineNumbers)
      putchar(0x0a);
    else
      printf(" %d\n",line_no);
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
