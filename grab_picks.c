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

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&col);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

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

    putchar(line[col]);
  }

  putchar(0x0a);

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
