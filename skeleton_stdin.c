#include <stdio.h>

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: skeleton_stdin filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  bool bStdin;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc > 2) {
    printf(usage);
    return 1;
  }
  else if (argc == 2)
    bStdin = false;
  else
    bStdin = true;

  if (bStdin)
    fptr = stdin;
  else if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    printf("%s\n",line);
  }

  if (!bStdin)
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
