#include <stdio.h>
#include <string.h>

#define TAB 0x09

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 32768
static char line[MAX_LINE_LEN];

#define MAX_COL_LEN 4096
static char column[MAX_COL_LEN];

static char usage[] = "usage: grabcol delim col infile\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int grab_col(char *line,int line_len,int line_no,int delim,int col,
  char *column,int max_col_len);

int main(int argc,char **argv)
{
  int delim;
  int col;
  FILE *fptr;
  int linelen;
  int line_no;
  int retval;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"tab"))
    delim = TAB;
  else
    delim = argv[1][0];

  sscanf(argv[2],"%d",&col);

  if (col < 1) {
    printf("col must be >= 1\n");
    return 2;
  }

  if ((fptr = fopen(argv[3],"r")) == NULL) {
    printf(couldnt_open,argv[3]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = grab_col(line,linelen,line_no,delim,col,column,MAX_COL_LEN);

    if (retval) {
      printf("grab_col() failed on line %d: %d\n",line_no,retval);
      return 4;
    }
    else
      printf("%s\n",column);
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
