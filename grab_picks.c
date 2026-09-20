#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: grab_picks col filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int col;
  FILE *fptr;
  int line_len;
  int line_no;
  int total_picks;
  int correct_picks;
  double percentage;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&col);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

  total_picks = 0;
  correct_picks = 0;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (col >= line_len) {
      printf("line %d is too short\n",line_no);
      return 3;
    }

    if (!(line_no % 2))
      total_picks++;

    if (line[col] == '1')
      correct_picks++;

    putchar(line[col]);
  }

  percentage = (double)correct_picks / (double)total_picks * (double)100;
  printf(" (%d of %d, %6.2lf%%\n",correct_picks,total_picks,percentage);

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
