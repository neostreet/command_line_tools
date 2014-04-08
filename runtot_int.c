#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: runtot_int (-initial_balbal) (-verbose) (-start_bal) (-start_and_end)\n"
"  (-offsetoffset) (-gain_loss) (-gain_only) (-loss_only) filename\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bStartBal;
  bool bStartAndEnd;
  bool bGainLoss;
  bool bGainOnly;
  bool bLossOnly;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  int runtot;
  int work;
  int runtot_gain;
  int runtot_loss;
  int num_gains;
  int num_losses;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  bVerbose = false;
  bStartBal = false;
  bStartAndEnd = false;
  bGainLoss = false;
  bGainOnly = false;
  bLossOnly = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12))
      sscanf(&argv[curr_arg][12],"%d",&runtot);
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
    else if (!strcmp(argv[curr_arg],"-gain_only"))
      bGainOnly = true;
    else if (!strcmp(argv[curr_arg],"-loss_only"))
      bLossOnly = true;
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

  if (bGainLoss && bGainOnly) {
    printf("can't specify both -gain_loss and -gain_only\n");
    return 4;
  }

  if (bGainLoss && bLossOnly) {
    printf("can't specify both -gain_loss and -loss_only\n");
    return 5;
  }

  if (bGainOnly && bLossOnly) {
    printf("can't specify both -gain_only and -loss_only\n");
    return 6;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 7;
  }

  if (bGainLoss) {
    runtot_gain = 0;
    runtot_loss = 0;
    num_gains = 0;
    num_losses = 0;
  }
  else if (bGainOnly) {
    runtot_gain = 0;
    num_gains = 0;
  }
  else if (bLossOnly) {
    runtot_loss = 0;
    num_losses = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(&line[offset],"%d",&work);

    if (!bStartBal && !bStartAndEnd) {
      if (!bGainOnly && !bLossOnly)
        runtot += work;

      if (bGainLoss) {
        if (work > 0) {
          runtot_gain += work;
          num_gains++;
        }
        else if (work < 0) {
          runtot_loss += work;
          num_losses++;
        }
      }
      else if (bGainOnly) {
        if (work > 0) {
          runtot_gain += work;
          num_gains++;
        }
      }
      else if (bLossOnly) {
        if (work < 0) {
          runtot_loss += work;
          num_losses++;
        }
      }
    }

    if (!bVerbose) {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%d\n",runtot);
      else if (bGainOnly)
        printf("%d (%d)\n",runtot_gain,num_gains);
      else if (bLossOnly)
        printf("%d (%d)\n",runtot_loss,num_losses);
      else {
        printf("%d (%d %d %d %d %d)\n",runtot,work,
          runtot_gain,runtot_loss,
          num_gains,num_losses);
      }
    }
    else {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%10d %s\n",runtot,line);
      else if (bGainOnly)
        printf("%10d %s\n",runtot_gain,line);
      else if (bLossOnly)
        printf("%10d %s\n",runtot_loss,line);
      else
        printf("%10d (%10d %10d) %s\n",runtot,runtot_gain,runtot_loss,line);
    }

    if (bStartBal || bStartAndEnd) {
      if (!bGainOnly && !bLossOnly)
        runtot += work;

      if (bGainLoss) {
        if (work > 0)
          runtot_gain += work;
        else if (work < 0)
          runtot_loss += work;
      }
      else if (bGainOnly) {
        if (work > 0)
          runtot_gain += work;
      }
      else if (bLossOnly) {
        if (work < 0)
          runtot_loss += work;
      }
    }
  }

  fclose(fptr);

  if (bStartAndEnd) {
    if (!bVerbose) {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%d\n",runtot);
      else if (bGainOnly)
        printf("%d\n",runtot_gain);
      else if (bLossOnly)
        printf("%d\n",runtot_loss);
      else
        printf("%d (%d %d)\n",runtot,runtot_gain,runtot_loss);
    }
    else {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%10d %s\n",runtot,line);
      else if (bGainOnly)
        printf("%10d %s\n",runtot_gain,line);
      else if (bLossOnly)
        printf("%10d %s\n",runtot_loss,line);
      else
        printf("%10d (%10d %10d) %s\n",runtot,runtot_gain,runtot_loss,line);
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
