#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[2][MAX_LINE_LEN];

static char usage[] = "usage: gen_trans_amounts file1 file2\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr[2];
  int line_len[2];
  int line_no;
  double amount;
  int debit;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++) {
    if ((fptr[n] = fopen(argv[n+1],"r")) == NULL) {
      printf(couldnt_open,argv[n+1]);
      return 2;
    }
  }

  line_no = 0;

  for ( ; ; ) {
    for (n = 0; n < 2; n++) {
      GetLine(fptr[n],line[n],&line_len[n],MAX_LINE_LEN);

      if (feof(fptr[n]))
        break;
    }

    if (n < 2)
      break;

    line_no++;

    sscanf(line[0],"%lf",&amount);

    if (!strcmp(line[1],"Debit"))
      amount *= (double)-1;
    else if (!strcmp(line[1],"Credit"))
      ;
    else {
      printf("unknown debit or credit on line %d\n",line_no);
      return 3;
    }

    printf("%8.2lf\n",amount);
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
