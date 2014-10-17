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

static char usage[] =
"usage: avg (-debug) (-verbose) (-runtot) (-abs) (-date_string)\n"
"  filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bRunTot;
  bool bAbs;
  bool bDateString;
  int retval;
  char *date_string;
  FILE *fptr;
  int linelen;
  int line_no;
  int tot;
  int work;
  double dwork;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bRunTot = false;
  bAbs = false;
  bDateString = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-runtot"))
      bRunTot = true;
    else if (!strcmp(argv[curr_arg],"-abs"))
      bAbs = true;
    else if (!strcmp(argv[curr_arg],"-date_string"))
      bDateString = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  if (bDateString) {
    getcwd(save_dir,_MAX_PATH);

    retval = get_date_from_path(save_dir,'/',2,&date_string);

    if (retval) {
      printf("get_date_from_path() failed: %d\n",retval);
      return 3;
    }
  }

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    if (bVerbose)
      printf("%s\n",argv[curr_arg]);

    line_no = 0;
    tot = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;
      sscanf(line,"%d",&work);

      if (bAbs) {
        if (work < 0)
          work *= -1;
      }

      tot += work;

      if (bVerbose) {
        dwork = (double)tot / (double)line_no;

        if (!bRunTot)
          printf("  %10d %9.2lf\n",work,dwork);
        else
          printf("  %10d %10d %9.2lf\n",work,tot,dwork);
      }
    }

    fclose(fptr);

    if (!bVerbose) {
      dwork = (double)tot / (double)line_no;

      if (!bRunTot) {
        if (!bDebug)
          printf("%9.2lf",dwork);
        else
          printf("%9.2lf (%d %d)",dwork,tot,line_no);
      }
      else
        printf("%10d %9.2lf",tot,dwork);

      if (!bDateString)
        putchar(0x0a);
      else
        printf(" %s\n",date_string);
    }
  }

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

static char sql_date_string[11];

static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(path);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (path[n] == slash_char) {
      slash_count++;

      if (slash_count == num_slashes)
        break;
    }
  }

  if (slash_count != num_slashes)
    return 1;

  if (path[n+5] != slash_char)
    return 2;

  strncpy(sql_date_string,&path[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&path[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&path[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

  return 0;
}
