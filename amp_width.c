#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: amp_width (-debug) (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_date_from_cwd(char *cwd,char **date_string_ptr);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  FILE *fptr;
  int retval;
  char *date_string;
  int line_len;
  int line_no;
  int max_balance;
  int max_balance_ix;
  int min_balance;
  int min_balance_ix;
  int ending_balance;
  int delta;
  int amp_width;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  if (bVerbose) {
    getcwd(save_dir,_MAX_PATH);
    retval = get_date_from_cwd(save_dir,&date_string);

    if (retval) {
      printf("get_date_from_cwd() failed: %d\n",retval);
      return 4;
    }
  }

  line_no = 0;
  ending_balance = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&delta);
    ending_balance += delta;

    if (line_no == 1) {
      max_balance = ending_balance;
      max_balance_ix = 0;
      min_balance = ending_balance;
      min_balance_ix = 0;
    }
    else if (ending_balance > max_balance) {
      max_balance = ending_balance;
      max_balance_ix = line_no - 1;
    }
    else if (ending_balance < min_balance) {
      min_balance = ending_balance;
      min_balance_ix = line_no - 1;
    }
  }

  fclose(fptr);

  if (max_balance_ix > min_balance_ix)
    amp_width = max_balance_ix - min_balance_ix + 1;
  else
    amp_width = min_balance_ix - max_balance_ix + 1;

  if (!bVerbose) {
    if (!bDebug)
      printf("%d\n",amp_width);
    else
      printf("%d (%d %d)\n",amp_width,min_balance_ix,max_balance_ix);
  }
  else {
    if (!bDebug)
      printf("%d %s\n",amp_width,date_string);
    else
      printf("%d %s (%d %d)\n",amp_width,date_string,min_balance_ix,max_balance_ix);
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

static char sql_date_string[11];

static int get_date_from_cwd(char *cwd,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(cwd);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (cwd[n] == '/') {
      slash_count++;

      if (slash_count == 2)
        break;
    }
  }

  if (slash_count != 2)
    return 1;

  if (cwd[n+5] != '/')
    return 2;

  strncpy(sql_date_string,&cwd[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&cwd[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&cwd[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

  return 0;
}
