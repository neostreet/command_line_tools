#include <stdio.h>
#include <string.h>

#define TAB 0x09

#define FALSE 0
#define TRUE  1

#define MAX_LINE_LEN 32768
static char line[MAX_LINE_LEN];

#define MAX_COL_LEN 4096
static char column[MAX_COL_LEN];

static char usage[] = "usage: grabcol (-verbose) (-skip_heading) delim col infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int grab_col(char *line,int line_len,int line_no,int delim,int col,
  char *column,int max_col_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bSkipHeading;
  int delim;
  int col;
  FILE *fptr;
  FILE *ofptr;
  int linelen;
  int line_no;
  int retval;

  if ((argc < 5) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bSkipHeading = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-skip_heading"))
      bSkipHeading = true;
    else
      break;
  }

  if (argc - curr_arg != 4) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg+2],argv[curr_arg+3])) {
    printf("infile must differ from outfile\n");
    return 3;
  }

  if (!strcmp(argv[curr_arg],"tab"))
    delim = TAB;
  else
    delim = argv[curr_arg][0];

  sscanf(argv[curr_arg+1],"%d",&col);

  if (col < 1) {
    printf("col must be >= 1\n");
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg+2],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+2]);
    return 5;
  }

  if ((ofptr = fopen(argv[curr_arg+3],"w")) == NULL) {
    printf(couldnt_open,argv[curr_arg+3]);
    return 6;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (bSkipHeading && (line_no == 1))
      continue;

    retval = grab_col(line,linelen,line_no,delim,col,column,MAX_COL_LEN);

    if (retval)
      printf("grab_col() failed on line %d: %d\n",line_no,retval);
    else {
      if (!bVerbose)
        fprintf(ofptr,"%s\n",column);
      else
        fprintf(ofptr,"%s %d\n",column,line_no);
    }
  }

  fclose(fptr);
  fclose(ofptr);

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
