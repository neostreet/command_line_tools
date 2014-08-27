#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: grab_num_denom filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int grab_num_denom(char *line,int line_len,int *num,int *denom);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  int num;
  int denom;
  int sum_nums;
  int sum_denoms;
  double dwork;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;
  sum_nums = 0;
  sum_denoms = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = grab_num_denom(line,line_len,&num,&denom);

    if (retval) {
      printf("grab_num_denom() failed on line %d: %d\n",line_no,retval);
      return 3;
    }

    sum_nums += num;
    sum_denoms += denom;
  }

  fclose(fptr);

  dwork = (double)sum_nums / (double)sum_denoms;

  printf("%lf (%d %d)\n",dwork,sum_nums,sum_denoms);

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

static int grab_num_denom(char *line,int line_len,int *num,int *denom)
{
  int n;

  for (n = 0; n < line_len; n++) {
    if (line[n] == '(')
      break;
  }

  if (n == line_len)
    return 1;

  n++;

  sscanf(&line[n],"%d %d",num,denom);

  return 0;
}
