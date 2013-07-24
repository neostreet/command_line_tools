#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[2][MAX_LINE_LEN];

static char usage[] = "usage: sub_files filename filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr[2];
  int line_len[2];
  int line_no;
  bool bBreak;
  int work[2];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++) {
    if ((fptr[n] = fopen(argv[1 + n],"r")) == NULL) {
      printf(couldnt_open,argv[1 +n]);
      return 2;
    }
  }

  line_no = 0;
  bBreak = false;

  for ( ; ; ) {
    for (n = 0; n < 2; n++) {
      GetLine(fptr[n],line[n],&line_len[n],MAX_LINE_LEN);

      if (feof(fptr[n])) {
        bBreak = true;
        break;
      }

      sscanf(&line[n][0],"%d",&work[n]);
    }

    if (bBreak)
      break;

    line_no++;

    printf("%d\n",work[0] - work[1]);
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
