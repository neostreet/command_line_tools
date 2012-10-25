#include <stdio.h>

#define MAX_LINE_LEN 256
char line[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int num_blns;
  FILE *fptr;
  int linel;
  int consecutive_blank_lines;

  if (argc != 3) {
    printf("usage: zapblns num_blns filename\n");
    return 1;
  }

  sscanf(argv[1],"%d",&num_blns);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf("couldn't open %s\n",argv[2]);
    return 2;
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

    if (consecutive_blank_lines < num_blns)
      printf("%s\n",line);
  }

  fclose(fptr);

  return 0;
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
