#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: addf (-debug) (-verbose) (-offsetoffset)\n"
"  (-datedatestring) (-get_date_from_cwd) (-pos_neg) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_date_from_cwd(char *cwd,char **date_string_ptr);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bHaveDateString;
  char *date_string;
  bool bGetDateFromCwd;
  bool bPosNeg;
  int retval;
  int offset;
  FILE *fptr;
  int linelen;
  int line_no;
  double work;
  double total;
  double positive_total;
  double negative_total;

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bHaveDateString = false;
  bGetDateFromCwd = false;
  bPosNeg = false;
  offset = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-offset",7))
      sscanf(&argv[curr_arg][7],"%d",&offset);
    else if (!strncmp(argv[curr_arg],"-date",5)) {
      date_string = &argv[curr_arg][5];
      bHaveDateString = true;
    }
    else if (!strcmp(argv[curr_arg],"-get_date_from_cwd"))
      bGetDateFromCwd = true;
    else if (!strcmp(argv[curr_arg],"-pos_neg"))
      bPosNeg = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bHaveDateString && bGetDateFromCwd) {
    printf("can't specify both -date and -get_date_from_cwd\n");
    return 3;
  }

  if (bDebug || bGetDateFromCwd)
    getcwd(save_dir,_MAX_PATH);

  if (bGetDateFromCwd) {
    retval = get_date_from_cwd(save_dir,&date_string);

    if (retval) {
      printf("get_date_from_cwd() failed: %d\n",retval);
      return 4;
    }

    bHaveDateString = true;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  line_no = 0;
  total = (double)0;

  if (bPosNeg) {
    positive_total = (double)0;
    negative_total = (double)0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%lf",&work);

    total += work;

    if (bVerbose)
      printf("%lf %s\n",total,line);

    if (bPosNeg) {
      if (work < (double)0)
        negative_total += work;
      else
        positive_total += work;
    }
  }

  fclose(fptr);

  if (!bPosNeg) {
    if (!bDebug) {
      if (!bHaveDateString)
        printf("%lf\n",total);
      else
        printf("%lf\t%s\n",total,date_string);
    }
    else {
      if (!bHaveDateString)
        printf("%lf %s/%s\n",total,save_dir,argv[curr_arg]);
      else
        printf("%lf\t%s %s/%s\n",total,date_string,save_dir,argv[curr_arg]);
    }
  }
  else {
    if (!bDebug) {
      if (!bHaveDateString)
        printf("%lf %lf %lf\n",total,positive_total,negative_total);
      else
        printf("%lf\t%s %lf %lf\n",total,date_string,positive_total,negative_total);
    }
    else {
      if (!bHaveDateString) {
        printf("%lf %lf %lf %s/%s\n",
          total,positive_total,negative_total,save_dir,argv[curr_arg]);
      }
      else {
        printf("%lf\t%s %lf %lf %s/%s\n",
          total,date_string,positive_total,negative_total,save_dir,argv[curr_arg]);
      }
    }
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
