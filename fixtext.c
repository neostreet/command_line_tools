#include <stdio.h>

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: fixtext filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char tempname[] = "fixtext.tmp";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  FILE *fptr2;
  int linelen;
  int bFixed;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"rb")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  if ((fptr2 = fopen(tempname,"wb")) == NULL) {
    printf(couldnt_open,tempname);
    fclose(fptr);
    return 3;
  }

  bFixed = FALSE;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (line[linelen - 1] == '\r')
      fprintf(fptr2,"%s\n",line);
    else {
      fprintf(fptr2,"%s\r\n",line);
      bFixed = TRUE;
    }
  }

  fclose(fptr);
  fclose(fptr2);

  if (bFixed) {
    remove(argv[1]);
    rename(tempname,argv[1]);
  }
  else
    remove(tempname);

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
