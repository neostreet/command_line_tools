#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: line_no_mod modulus ix filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int modulus;
  int ix;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&modulus);
  sscanf(argv[2],"%d",&ix);

  if ((fptr = fopen(argv[3],"r")) == NULL) {
    printf(couldnt_open,argv[3]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if ((line_no % modulus) == ix)
      printf("%s\n",line);

    line_no++;
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
