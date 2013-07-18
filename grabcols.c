#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB 0x09

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 8192
char line[MAX_LINE_LEN];

#define MAX_COL_LEN 1024
char column[MAX_COL_LEN];

static char usage[] = "usage: grabcols delim col (col ...) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int grab_col(char *line,int line_len,int line_no,int delim,int col,
  char *column,int max_col_len);

int main(int argc,char **argv)
{
  int n;
  int delim;
  int num_cols;
  int *cols;
  FILE *fptr;
  int linelen;
  int line_no;
  int retval;

  if (argc < 4) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"tab"))
    delim = TAB;
  else
    delim = argv[1][0];

  num_cols = argc - 3;

  cols = (int *)malloc(num_cols * sizeof (int));

  if (cols == NULL) {
    printf("couldn't malloc %d ints\n",num_cols);
    return 2;
  }

  for (n = 0; n < num_cols; n++) {
    sscanf(argv[2+n],"%d",&cols[n]);

    if (cols[n] < 1) {
      printf("col must be >= 1\n");
      return 3;
    }
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    free(cols);
    return 4;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    for (n = 0; n < num_cols; n++) {
      retval = grab_col(line,linelen,line_no,delim,cols[n],column,MAX_COL_LEN);

      if (retval)
        printf("grab_col() failed on line %d: %d\n",line_no,retval);
      else {
        printf("%s",column);

        if (n < num_cols - 1)
          putchar(',');
        else
          putchar(0x0a);
      }
    }
  }

  fclose(fptr);

  free(cols);

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

int grab_col(char *line,int line_len,int line_no,int delim,int col,
  char *column,int max_col_len)
{
  int m;
  int n;
  int bInDoubleQuotes;

  /* first, remove double quotes and all delimiters contained within them */

  m = 0;
  bInDoubleQuotes = 0;

  for (n = 0; n < line_len; n++) {
    if (line[n] == '"') {
      if (!bInDoubleQuotes)
        bInDoubleQuotes = TRUE;
      else
        bInDoubleQuotes = FALSE;

      continue;
    }

    if ((!bInDoubleQuotes) || (line[n] != delim)) {
      if (m != n)
        line[m] = line[n];

      m++;
    }
  }

  line[m] = 0;
  line_len = m;

  m = 0;

  for (n = 0; n < col - 1; n++) {
    for ( ; m < line_len; ) {
      if (line[m++] == delim)
        break;
    }

    if (m == line_len)
      return 1;
  }

  n = 0;

  for ( ; n < max_col_len - 1; ) {
    if ((m == line_len) || (line[m] == delim))
      break;

    column[n++] = line[m++];
  }

  if ((m != line_len) && (line[m] != delim))
    return 2;

  column[n] = 0;

  return 0;
}
