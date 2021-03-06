#include <stdio.h>

static char usage[] = "usage: zorch infile\n";

#define MAX_LINE_LEN 256
char line[MAX_LINE_LEN];
char line2[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void zorch_line(char *in_line,char *out_line);

int main(int argc,char **argv)
{
  FILE *fptr;
  int linel;

  if (argc != 2) {
    printf(usage);
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

    zorch_line(line,line2);
    printf("%s\n",line2);
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

void zorch_line(char *in_line,char *out_line)
{
  int m;
  int n;
  int bHaveNonSpace;
  int last_char;

  m = 0;
  n = 0;

  bHaveNonSpace = 0;
  last_char = 0;

  for ( ; (in_line[m]); m++) {
    if (in_line[m] != ' ') {
      if (!bHaveNonSpace) {
        out_line[n++] = in_line[m];
        bHaveNonSpace = 1;
      }
      else {
        if (last_char == ' ')
          out_line[n++] = ' ';

        out_line[n++] = in_line[m];
      }
    }

    last_char = in_line[m];
  }

  out_line[n] = 0;
}
