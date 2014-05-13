#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: runtot_double (-initial_balbal) (-terse) (-verbose) (-start_bal) (-start_and_end) (-offsetoffset)\n"
"  (-gain_loss) filename\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bTerse;
  bool bVerbose;
  bool bStartBal;
  bool bStartAndEnd;
  bool bGainLoss;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  double runtot;
  double work;
  double runtot_gain;
  double runtot_loss;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  bTerse = false;
  bVerbose = false;
  bStartBal = false;
  bStartAndEnd = false;
  bGainLoss = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12))
      sscanf(&argv[curr_arg][12],"%d",&runtot);
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-start_bal"))
      bStartBal = true;
    else if (!strcmp(argv[curr_arg],"-start_and_end"))
      bStartAndEnd = true;
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strcmp(argv[curr_arg],"-gain_loss"))
      bGainLoss = true;
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

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 5;
  }

  if (bGainLoss) {
    runtot_gain = 0;
    runtot_loss = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(&line[offset],"%lf",&work);

    if (!bStartBal && !bStartAndEnd) {
      runtot += work;

      if (bGainLoss) {
        if (work > 0)
          runtot_gain += work;
        else if (work < 0)
          runtot_loss += work;
      }
    }

    if (bTerse)
      ;
    else if (!bVerbose) {
      if (!bGainLoss)
        printf("%lf\n",runtot);
      else
        printf("%lf (%lf %lf)\n",runtot,runtot_gain,runtot_loss);
    }
    else {
      if (!bGainLoss)
        printf("%lf %s\n",runtot,line);
      else
        printf("%lf (%lf %lf) %s\n",runtot,runtot_gain,runtot_loss,line);
    }

    if (bStartBal || bStartAndEnd) {
      runtot += work;

      if (bGainLoss) {
        if (work > 0)
          runtot_gain += work;
        else if (work < 0)
          runtot_loss += work;
      }
    }
  }

  fclose(fptr);

  if (bTerse || bStartAndEnd) {
    if (!bVerbose) {
      if (!bGainLoss)
        printf("%lf\n",runtot);
      else
        printf("%lf (%lf %lf)\n",runtot,runtot_gain,runtot_loss);
    }
    else {
      if (!bGainLoss)
        printf("%lf %s\n",runtot,line);
      else
        printf("%lf (%lf %lf) %s\n",runtot,runtot_gain,runtot_loss,line);
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
