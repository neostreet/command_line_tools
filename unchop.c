#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 8192
char line[MAX_LINE_LEN];

static char usage[] = "usage: unchop (-delim_charc) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  bool bDelimChar;
  char delim_char;
  FILE *fptr;
  int linelen;
  int line_no;

  if ((argc != 2) && (argc != 3)) {
    printf(usage);
    return 1;
  }

  if (argc == 3) {
    if ((strlen(argv[1]) == 12) && !strncmp(argv[1],"-delim_char",11)) {
      delim_char = argv[1][11];

      if (delim_char == '@')
        delim_char = ' ';

      bDelimChar = true;
    }
    else {
      printf(usage);
      return 2;
    }
  }
  else
    bDelimChar = false;

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (bDelimChar) {
      if (line_no == 1)
        printf("%s",line);
      else
        printf("%c%s",delim_char,line);
    }
    else
      printf("%s",line);
  }

  putchar(0x0a);

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
