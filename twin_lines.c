#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[3][MAX_LINE_LEN];

static char usage[] = "usage: twin_lines filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int curr_ix;
  int compare_ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    curr_ix = line_no % 3;

    GetLine(fptr,line[curr_ix],&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no >= 3) {
      switch (curr_ix) {
        case 0:
          compare_ix = 1;

          break;
        case 1:
          compare_ix = 2;

          break;
        case 2:
          compare_ix = 0;

          break;
      }

      if (!strcmp(line[curr_ix],line[compare_ix]))
        printf("%7d %s\n",line_no,line[curr_ix]);
    }
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
