#include <stdio.h>

#define MAX_LINE_LEN 256
char line[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

void main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int linel;
  int consecutive_blank_lines;

  if (argc != 2) {
    printf("usage: zapblns filename\n");
    return;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return;
  }

  consecutive_blank_lines = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linel,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (!linel)
      consecutive_blank_lines++;
    else
      consecutive_blank_lines = 0;

    if (consecutive_blank_lines < 2)
      printf("%s\n",line);
  }

  fclose(fptr);
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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
