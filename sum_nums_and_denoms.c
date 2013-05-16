#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: sum_nums_and_denoms filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int nums_total;
  int denoms_total;
  int num;
  int denom;
  double work;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;
  nums_total = 0;
  denoms_total = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    for (n = 0; n < line_len; n++) {
      if (line[n] == '(')
        break;
    }

    if (n == line_len)
      continue;

    n++;
    m = n;

    for ( ; n < line_len; n++) {
      if (line[n] == ')')
        break;
    }

    if (n == line_len)
      continue;

    line[n] = 0;

    sscanf(&line[m],"%d %d",&num,&denom);

    nums_total += num;
    denoms_total += denom;
  }

  fclose(fptr);

  work = (double)nums_total / (double)denoms_total;

  printf("%8.6lf ",work);
  printf("(%d %d)",
    nums_total,denoms_total);

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
