#include <stdio.h>

#define TAB 0x09

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void zaptrail(char *line,int linel);

int main(int argc,char **argv)
{
  FILE *fptr;
  int linel;

  if (argc != 2) {
    printf("usage: zaptrail filename\n");
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return 2;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linel,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    zaptrail(line,linel);
    printf("%s\n",line);
  }

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

void zaptrail(char *line,int linel)
{
  int n;

  for (n = linel - 1; (n >= 0); n--) {
    if ((line[n] != ' ') && (line[n] != TAB))
      break;
  }

  line[n+1] = 0;
}
