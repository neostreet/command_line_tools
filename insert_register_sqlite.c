#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "sqlite3.h"

#define TAB 0x09

#define FALSE 0
#define TRUE  1

#define ARG_DELIM     0
#define ARG_FILE      1
#define ARG_DATABASE  2
#define ARG_TABLE     3
#define NUM_ARGS      4

#define TRANS_DATE_COL        1
#define CHECK_NUMBER_COL      2
#define DESCRIPTION_COL       3
#define WITHDRAWAL_AMOUNT_COL 4
#define DEPOSIT_AMOUNT_COL    5
#define ADDL_INFO_COL         6

static int bDebug;

static sqlite3 *db;
static double balance;

static char balance_query_buf[256];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#define MAX_TRANS_DATE_LEN 20
static char trans_date[MAX_TRANS_DATE_LEN];
static char trans_date2[MAX_TRANS_DATE_LEN];

#define MAX_CHECK_NUMBER_LEN 10
static char check_number[MAX_CHECK_NUMBER_LEN];

#define MAX_DESCRIPTION_LEN 41
static char description[MAX_DESCRIPTION_LEN];

#define MAX_WITHDRAWAL_AMOUNT_LEN 20
static char withdrawal_amount_str[MAX_WITHDRAWAL_AMOUNT_LEN];

#define MAX_DEPOSIT_AMOUNT_LEN 20
static char deposit_amount_str[MAX_DEPOSIT_AMOUNT_LEN];

#define MAX_ADDL_INFO_LEN 101
static char addl_info[MAX_ADDL_INFO_LEN];

static char usage[] =
"usage: insert_register (-debug) delim file user password database table\n";
static char login_to_sqlite_failed[] = "login_to_sqlite() error: %d";
static char couldnt_open[] = "couldn't open %s\n";

static char grab_col_failure[] =
"grab_col() failed for col %d of line %d: %d\n";

static int login_to_sqlite(char *database);
static int callback(void *NotUsed,int argc,char **argv,char **azColName);
static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int grab_col(char *line,int *line_len_pt,int line_no,int delim,int col,
  char *column,int max_col_len);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  char *zErrMsg;
  int delim;
  FILE *fptr;
  int linelen;
  int line_no;
  int retval;
  double withdrawal_amount;
  double deposit_amount;

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

  retval = login_to_sqlite(argv[curr_arg+ARG_DATABASE]);

  if (retval) {
    printf(login_to_sqlite_failed,retval);
    return 3;
  }

  sprintf(balance_query_buf,
    "select sum(withdrawal_amount)+sum(deposit_amount) "
    "from %s",argv[curr_arg+ARG_TABLE]);

  retval = sqlite3_exec(db,balance_query_buf,callback,0,&zErrMsg);

  if (retval != SQLITE_OK)
    return 4;

  if (!strcmp(argv[curr_arg+ARG_DELIM],"tab"))
    delim = TAB;
  else
    delim = argv[curr_arg+ARG_DELIM][0];

  if ((fptr = fopen(argv[curr_arg+ARG_FILE],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+ARG_FILE]);
    return 6;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = grab_col(line,&linelen,line_no,delim,
      TRANS_DATE_COL,trans_date,MAX_TRANS_DATE_LEN);

    if (retval) {
      printf(grab_col_failure,TRANS_DATE_COL,line_no,retval);
      break;
    }

    retval = grab_col(line,&linelen,line_no,delim,
      CHECK_NUMBER_COL,check_number,MAX_CHECK_NUMBER_LEN);

    if (retval) {
      printf(grab_col_failure,CHECK_NUMBER_COL,line_no,retval);
      break;
    }

    retval = grab_col(line,&linelen,line_no,delim,
      DESCRIPTION_COL,description,MAX_DESCRIPTION_LEN);

    if (retval) {
      printf(grab_col_failure,DESCRIPTION_COL,line_no,retval);
      break;
    }

    retval = grab_col(line,&linelen,line_no,delim,
      WITHDRAWAL_AMOUNT_COL,withdrawal_amount_str,MAX_WITHDRAWAL_AMOUNT_LEN);

    if (retval) {
      printf(grab_col_failure,WITHDRAWAL_AMOUNT_COL,line_no,retval);
      break;
    }

    withdrawal_amount = (double)0.0;
    sscanf(withdrawal_amount_str,"%lf",&withdrawal_amount);

    retval = grab_col(line,&linelen,line_no,delim,
      DEPOSIT_AMOUNT_COL,deposit_amount_str,MAX_DEPOSIT_AMOUNT_LEN);

    if (retval) {
      printf(grab_col_failure,DEPOSIT_AMOUNT_COL,line_no,retval);
      break;
    }

    deposit_amount = (double)0.0;
    sscanf(deposit_amount_str,"%lf",&deposit_amount);

    balance += withdrawal_amount + deposit_amount;

    retval = grab_col(line,&linelen,line_no,delim,
      ADDL_INFO_COL,addl_info,MAX_ADDL_INFO_LEN);

    if (retval) {
      printf(grab_col_failure,ADDL_INFO_COL,line_no,retval);
      break;
    }

    for (n = 0; n < 4; n++)
      trans_date2[n] = trans_date[6+n];

    trans_date2[4] = '-';

    for (n = 0; n < 2; n++)
      trans_date2[5+n] = trans_date[n];

    trans_date2[7] = '-';

    for (n = 0; n < 2; n++)
      trans_date2[8+n] = trans_date[3+n];

    trans_date2[10] = 0;

    if (!strlen(check_number)) {
      if (!strlen(addl_info)) {
        printf("insert into %s (trans_date,description,"
          "withdrawal_amount,deposit_amount,balance) values (\n",
          argv[curr_arg+ARG_TABLE]);
        printf("  '%s','%s',%lf,%lf,%lf);\n",trans_date2,description,
          withdrawal_amount,deposit_amount,balance);
      }
      else {
        printf("insert into %s (trans_date,description,"
          "withdrawal_amount,deposit_amount,balance,addl_info) values (\n",
          argv[curr_arg+ARG_TABLE]);
        printf("  '%s','%s',%lf,%lf,%lf,'%s');\n",trans_date2,description,
          withdrawal_amount,deposit_amount,balance,addl_info);
      }
    }
    else {
      if (!strlen(addl_info)) {
        printf("insert into %s (trans_date,check_number,description,"
          "withdrawal_amount,deposit_amount,balance) values (\n",
          argv[curr_arg+ARG_TABLE]);
        printf("  '%s',%s,'%s',%lf,%lf,%lf);\n",trans_date2,check_number,
          description,withdrawal_amount,deposit_amount,balance);
      }
      else {
        printf("insert into %s (trans_date,check_number,description,"
          "withdrawal_amount,deposit_amount,balance,addl_info) values (\n",
          argv[curr_arg+ARG_TABLE]);
        printf("  '%s',%s,'%s',%lf,%lf,%lf,'%s');\n",trans_date2,check_number,
          description,withdrawal_amount,deposit_amount,balance,addl_info);
      }
    }
  }

  fclose(fptr);

  sqlite3_close(db);

  return 0;
}

static int login_to_sqlite(char *database)
{
  int retval;

  retval = sqlite3_open(database,&db);

  return retval;
}

static int callback(void *NotUsed,int argc,char **argv,char **azColName)
{
  int n;
  double dwork;

  balance = (double)0.0;

  for (n = 0; n < argc; n++) {
    sscanf(argv[n],"%lf",&dwork);
    balance += dwork;
  }

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
  static int dbg_line_no;
  static int dbg_col;
  int dbg;

  if ((line_no == dbg_line_no) && (col == dbg_col))
    dbg = 1;

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

  for ( ; n < max_col_len - 1; ) {
    if ((m == line_len) || (line[m] == delim))
      break;

    column[n++] = line[m++];
  }

  if ((m != line_len) && (line[m] != delim) && (col != ADDL_INFO_COL))
    return 2;

  column[n] = 0;

  if (bDebug)
    printf("dbg: col %d, max_col_len %d,len %d\n",col,max_col_len-1,n);

  return 0;
}
