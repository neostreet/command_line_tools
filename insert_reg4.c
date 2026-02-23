#include <stdio.h>
#include <string.h>

#define TAB 0x09

#define FALSE 0
#define TRUE  1

#define ARG_DELIM     0
#define ARG_FILE      1
#define ARG_DATABASE  2
#define ARG_TABLE     3
#define NUM_ARGS      4

#define TRANS_DATE_COL        1
#define TYPE_COL              2
#define DESCRIPTION_COL       3
#define AMOUNT_COL            4
#define ACCOUNT_COL           5
#define NOTES_COL             6
#define BALANCE_COL           7
#define RAW_DESCRIPTION_COL   8

static int bDebug;

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#define MAX_TRANS_DATE_LEN 25
static char trans_date[MAX_TRANS_DATE_LEN+1];

#define MAX_DESCRIPTION_LEN 160
static char description[MAX_DESCRIPTION_LEN+1];

#define MAX_AMOUNT_LEN 20
static char amount_str[MAX_AMOUNT_LEN+1];

static char usage[] =
"usage: insert_reg4 (-debug) delim file database table\n";
static char couldnt_open[] = "couldn't open %s\n";

static char grab_col_failure[] =
"grab_col() failed for col %d of line %d: %d\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int grab_col(char *line,int *line_len_pt,int line_no,int delim,int col,
  char *column,int max_col_len);

static int dbg_line_no;
static int dbg_col;

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int delim;
  FILE *fptr;
  int linelen;
  int line_no;
  int retval;
  int dbg;

  if ((argc < NUM_ARGS + 1) || (argc > NUM_ARGS + 2)) {
    printf(usage);
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
    else
      break;
  }

  if (argc - curr_arg != NUM_ARGS) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg+ARG_DELIM],"tab"))
    delim = TAB;
  else
    delim = argv[curr_arg+ARG_DELIM][0];

  if ((fptr = fopen(argv[curr_arg+ARG_FILE],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+ARG_FILE]);
    return 3;
  }

  line_no = 0;

  printf("use %s\n\n",argv[curr_arg+ARG_DATABASE]);

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no == dbg_line_no)
      dbg = 1;

    if (!strncmp(&line[1],"Posted Date",11))
      continue;

    retval = grab_col(line,&linelen,line_no,delim,
      TRANS_DATE_COL,trans_date,MAX_TRANS_DATE_LEN);

    if (retval) {
      printf(grab_col_failure,TRANS_DATE_COL,line_no,retval);
      break;
    }

    trans_date[10] = 0;

    retval = grab_col(line,&linelen,line_no,delim,
      RAW_DESCRIPTION_COL,description,MAX_DESCRIPTION_LEN);

    if (retval) {
      printf(grab_col_failure,DESCRIPTION_COL,line_no,retval);
      break;
    }

    retval = grab_col(line,&linelen,line_no,delim,
      AMOUNT_COL,amount_str,MAX_AMOUNT_LEN);

    if (retval) {
      printf(grab_col_failure,AMOUNT_COL,line_no,retval);
      break;
    }

    printf("insert into %s (trans_date,description,"
      "amount) values (\n",
      argv[curr_arg+ARG_TABLE]);
    printf("  '%s','%s',%s);\n",trans_date,description,amount_str);
  }

  fclose(fptr);

  printf("\nquit\n");

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

int grab_col(char *line,int *line_len_pt,int line_no,int delim,int col,
  char *column,int max_col_len)
{
  int m;
  int n;
  int line_len;
  int bInDoubleQuotes;
  int dbg;

  if (line_no == dbg_line_no)
    dbg = 1;

  if ((line_no == dbg_line_no) && (col == dbg_col))
    dbg = 2;

  line_len = *line_len_pt;

  if (col == TRANS_DATE_COL) {
    /* remove double quotes and all delimiters contained within them */

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

      if ((line[n] != '\'') && ((!bInDoubleQuotes) || (line[n] != delim))) {
        if (m != n)
          line[m] = line[n];

        m++;
      }
    }

    line[m] = 0;
    line_len = m;
    *line_len_pt = m;
  }

  n = 0;
  m = 0;

  for ( ; n < col - 1; ) {
    for ( ; m < line_len; ) {
      if (line[m++] == delim) {
        n++;
        break;
      }
    }

    if ((m == line_len) && (n < col - 1))
      return 1;
  }

  n = 0;

  for ( ; n < max_col_len; ) {
    if ((m == line_len) || (line[m] == delim))
      break;

    column[n++] = line[m++];
  }

  if ((m != line_len) && (line[m] != delim))
    return 2;

  column[n] = 0;

  if (bDebug)
    printf("dbg: col %d, max_col_len %d,len %d\n",col,max_col_len-1,n);

  return 0;
}
