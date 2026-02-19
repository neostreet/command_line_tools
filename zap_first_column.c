#include <stdio.h>

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "usage: zap_first_column filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static char *zap_first_column(char *line,int line_len);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    printf("%s\n",zap_first_column(line,line_len));
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

static char *zap_first_column(char *line,int line_len)
{
  int n;

  for (n = 0; n < line_len; n++) {
    if (line[n] == ',')
      break;
  }

  if (n < line_len)
    return &line[n+1];

  return line;
}
