#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: case { upper | lower } filename "
  "(filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";
static char tempname[] = "case.tmp";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
char *case_line(char *line,int line_len,int bUpper);

int main(int argc,char **argv)
{
  int n;
  int bUpper;
  FILE *fptr;
  FILE *fptr2;
  int line_len;

  if (argc < 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"upper"))
    bUpper = TRUE;
  else if (!strcmp(argv[1],"lower"))
    bUpper = FALSE;
  else {
    printf(usage);
    return 2;
  }

  for (n = 2; n < argc; n++) {
    if ((fptr = fopen(argv[n],"r")) == NULL) {
      printf(couldnt_open,argv[n]);
      continue;
    }

    if ((fptr2 = fopen(tempname,"w")) == NULL) {
      printf(couldnt_open,tempname);
      fclose(fptr);
      break;
    }

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      fprintf(fptr2,"%s\n",case_line(line,line_len,bUpper));
    }

    fclose(fptr);
    fclose(fptr2);

    remove(argv[n]);
    rename(tempname,argv[n]);
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

char *case_line(char *line,int line_len,int bUpper)
{
  int n;
  int chara;

  for (n = 0; n < line_len; n++) {
    chara = line[n];

    if (bUpper) {
      if ((chara >= 'a') && (chara <= 'z'))
        line[n] = chara - 'a' + 'A';
    }
    else {
      if ((chara >= 'A') && (chara <= 'Z'))
        line[n] = chara - 'A' + 'a';
    }
  }

  return line;
}
