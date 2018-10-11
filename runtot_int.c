#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

static char usage[] =
"usage: runtot_int (-initial_balbal) (-verbose) (-start_bal) (-start_and_end)\n"
"  (-offsetoffset) (-gain_loss) (-gain_only) (-loss_only) (-abs_value)\n"
"  (-line_numbers) (-tot_at_end) (-final_negative)\n"
"  (-only_blue) (-underwater_only) (-coalesce) (-debug) filename\n";

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
  bool bAbsValue;
  bool bLineNumbers;
  bool bTotAtEnd;
  bool bFinalNegative;
  bool bOnlyBlue;
  bool bUnderwaterOnly;
  bool bCoalesce;
  bool bDebug;
  bool bHaveBlue;
  bool bPrinted;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  int runtot;
  int work;
  int last_work;
  int dbg_work;
  int dbg;
  int runtot_gain;
  int runtot_loss;
  int num_gains;
  int num_losses;
  int final_negative;
  int blue_val;

  if ((argc < 2) || (argc > 18)) {
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
  bAbsValue = false;
  bLineNumbers = false;
  bTotAtEnd = false;
  bFinalNegative = false;
  bOnlyBlue = false;
  bUnderwaterOnly = false;
  bCoalesce = false;
  bDebug = false;
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
    else if (!strcmp(argv[curr_arg],"-abs_value"))
      bAbsValue = true;
    else if (!strcmp(argv[curr_arg],"-line_numbers"))
      bLineNumbers = true;
    else if (!strcmp(argv[curr_arg],"-tot_at_end"))
      bTotAtEnd = true;
    else if (!strcmp(argv[curr_arg],"-final_negative")) {
      bFinalNegative = true;
      final_negative = 0;
    }
    else if (!strcmp(argv[curr_arg],"-only_blue"))
      bOnlyBlue = true;
    else if (!strcmp(argv[curr_arg],"-underwater_only"))
      bUnderwaterOnly = true;
    else if (!strcmp(argv[curr_arg],"-coalesce"))
      bCoalesce = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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

  line_no = 0;

  if (bOnlyBlue)
    blue_val = runtot;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (bDebug)
      printf("%s\n",line);

    bPrinted = false;

    sscanf(&line[offset],"%d",&work);

    if (work == dbg_work)
      dbg = 1;

    if (bCoalesce) {
      if (line_no == 1) {
        runtot = work;
        last_work = work;
        continue;
      }

      if (((last_work <= 0) && (work <= 0)) ||
          ((last_work >= 0) && (work >= 0))) {
        runtot += work;
      }
      else {
        if (!bVerbose)
          printf("%s%d\n",(bDebug ? "  " : ""),runtot);
        else
          printf("%d %s\n",runtot,line);

        runtot = work;
      }

      last_work = work;

      continue;
    }

    if (bAbsValue) {
      if (work < 0)
        work *= -1;
    }

    if (!bStartBal && !bStartAndEnd) {
      if (!bGainOnly && !bLossOnly) {
        runtot += work;

        if (bOnlyBlue) {
          if (runtot > blue_val) {
            blue_val = runtot;
            bHaveBlue = true;
          }
          else
            bHaveBlue = false;
        }
      }

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

    if (bFinalNegative) {
      if (runtot < 0) {
        final_negative = runtot;
        strcpy(save_line,line);
      }

      continue;
    }
    else if (!bVerbose) {
      if (!bGainLoss && !bGainOnly && !bLossOnly && !bUnderwaterOnly) {
        if (!bOnlyBlue || bHaveBlue) {
          printf("%d",runtot);
          bPrinted = true;
        }
      }
      else if (bGainOnly) {
        if (work > 0) {
          printf("%d",runtot_gain);
          bPrinted = true;
        }
      }
      else if (bLossOnly) {
        if (work < 0) {
          printf("%d",runtot_loss);
          bPrinted = true;
        }
      }
      else if (bGainLoss) {
        printf("%d (%d %d %d %d)",runtot,
          runtot_gain,num_gains,
          runtot_loss,num_losses);
          bPrinted = true;
      }
      else {
        if (runtot < 0) {
          if (!bOnlyBlue || bHaveBlue) {
            printf("%d",runtot);
            bPrinted = true;
          }
        }
      }
    }
    else {
      if (!bGainLoss && !bGainOnly && !bLossOnly && !bUnderwaterOnly) {
        if (!bOnlyBlue || bHaveBlue) {
          if (!bTotAtEnd)
            printf("%d %s",runtot,line);
          else
            printf("%s %d",line,runtot);
        }
        bPrinted = true;
      }
      else if (bGainOnly) {
        if (work > 0)
          printf("%d %s",runtot_gain,line);
        bPrinted = true;
      }
      else if (bLossOnly) {
        if (work < 0)
          printf("%d %s",runtot_loss,line);
        bPrinted = true;
      }
      else if (bGainLoss) {
        printf("%d (%d %d %d %d) %s",runtot,
          runtot_gain,num_gains,
          runtot_loss,num_losses,
          line);
        bPrinted = true;
      }
      else {
        if (runtot < 0) {
          if (!bOnlyBlue || bHaveBlue) {
            printf("%d %s",runtot,line);
            bPrinted = true;
          }
        }
      }
    }

    if (bPrinted) {
      if (!bOnlyBlue || bHaveBlue) {
        if (!bLineNumbers)
          putchar(0x0a);
        else
          printf(" %d\n",line_no);
      }
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

  if (bCoalesce)
    printf("%s%d\n",(bDebug ? "  " : ""),runtot);

  if (bFinalNegative) {
    if (final_negative < 0) {
      printf("%d %s\n",final_negative,save_line);
    }
  }
  else if (bStartAndEnd) {
    if (!bVerbose) {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%d",runtot);
      else if (bGainOnly) {
        if (work > 0)
          printf("%d",runtot_gain);
      }
      else if (bLossOnly) {
        if (work < 0)
        printf("%d",runtot_loss);
      }
      else {
        printf("%d (%d %d %d %d)",runtot,
          runtot_gain,num_gains,
          runtot_loss,num_losses);
      }
    }
    else {
      if (!bGainLoss && !bGainOnly && !bLossOnly)
        printf("%d %s",runtot,line);
      else if (bGainOnly) {
        if (work > 0)
          printf("%d (%d) %s",runtot_gain,num_gains,line);
      }
      else if (bLossOnly) {
        if (work < 0)
          printf("%d (%d) %s",runtot_loss,num_losses,line);
      }
      else {
        printf("%d (%d %d %d %d) %s",runtot,
          runtot_gain,num_gains,
          runtot_loss,num_losses,
          line);
      }
    }

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
