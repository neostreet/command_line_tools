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

static char usage[] = "usage: addf_int (-debug) (-verbose) (-offsetoffset)\n"
"  (-datedatestring) (-get_date_from_cwd) (-pos_neg) (-counts) (-abs)\n"
"  (-neg_only) (-pos_only) (-last_is_lowest) filename\n";
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
  bool bCounts;
  bool bAbs;
  bool bNegOnly;
  bool bPosOnly;
  bool bLastIsLowest;
  int retval;
  int offset;
  FILE *fptr;
  int linelen;
  int line_no;
  int work;
  int total;
  int min_total;
  int count;
  int positive_total;
  int positive_count;
  int negative_total;
  int negative_count;

  if ((argc < 2) || (argc > 13)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bHaveDateString = false;
  bGetDateFromCwd = false;
  bPosNeg = false;
  bCounts = false;
  bAbs = false;
  bNegOnly = false;
  bPosOnly = false;
  bLastIsLowest = false;
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
    else if (!strcmp(argv[curr_arg],"-counts"))
      bCounts = true;
    else if (!strcmp(argv[curr_arg],"-abs"))
      bAbs = true;
    else if (!strcmp(argv[curr_arg],"-neg_only"))
      bNegOnly = true;
    else if (!strcmp(argv[curr_arg],"-pos_only"))
      bPosOnly = true;
    else if (!strcmp(argv[curr_arg],"-last_is_lowest"))
      bLastIsLowest = true;
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

  if (bNegOnly && bPosOnly) {
    printf("can't specify both -neg_only and -pos_only\n");
    return 4;
  }

  if (bDebug || bGetDateFromCwd)
    getcwd(save_dir,_MAX_PATH);

  if (bGetDateFromCwd) {
    retval = get_date_from_cwd(save_dir,&date_string);

    if (retval) {
      printf("get_date_from_cwd() failed: %d\n",retval);
      return 5;
    }

    bHaveDateString = true;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 6;
  }

  line_no = 0;

  total = 0;

  if (bCounts)
    count = 0;

  if (bPosNeg) {
    positive_total = 0;
    negative_total = 0;

    if (bCounts) {
      negative_count = 0;
      positive_count = 0;
    }
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(&line[offset],"%d",&work);

    if (!bAbs) {
      if (bNegOnly) {
        if (work < 0)
          total += work;
      }
      else if (bPosOnly) {
        if (work > 0)
          total += work;
      }
      else {
        total += work;

        if (bLastIsLowest && ((line_no == 1) || (total < min_total)))
          min_total = total;
      }
    }
    else {
      if (work < 0)
        total -= work;
      else
        total += work;
    }

    if (bCounts)
      count++;

    if (bPosNeg) {
      if (work < 0) {
        negative_total += work;

        if (bCounts)
          negative_count++;
      }
      else if (work > 0) {
        positive_total += work;

        if (bCounts)
          positive_count++;
      }
    }

    if (bVerbose) {
      if (!bPosNeg)
        printf("%d %s\n",total,line);
      else
        printf("%d %d %d %s\n",total,positive_total,negative_total,line);
    }
  }

  fclose(fptr);

  if (!bVerbose) {
    if (!bPosNeg) {
      if (!bDebug) {
        if (!bHaveDateString) {
          if (!bCounts) {
            if (!bLastIsLowest || (total == min_total))
              printf("%d\n",total);
          }
          else
            printf("%d (%d)\n",total,count);
        }
        else {
          if (!bCounts)
            printf("%d\t%s\n",total,date_string);
          else
            printf("%d (%d)\t%s\n",total,count,date_string);
        }
      }
      else {
        if (!bHaveDateString) {
          if (!bCounts) {
            if (!bLastIsLowest || (total == min_total))
              printf("%d %s/%s\n",total,save_dir,argv[curr_arg]);
          }
          else
            printf("%d (%d) %s/%s\n",total,count,save_dir,argv[curr_arg]);
        }
        else {
          if (!bCounts)
            printf("%d\t%s %s/%s\n",total,date_string,save_dir,argv[curr_arg]);
          else
            printf("%d (%d)\t%s %s/%s\n",total,count,date_string,save_dir,argv[curr_arg]);
        }
      }
    }
    else {
      if (!bDebug) {
        if (!bHaveDateString) {
          if (!bCounts)
            printf("%d %d %d\n",total,positive_total,negative_total);
          else {
            printf("%d (%d) %d (%d) %d (%d)\n",
              total,count,positive_total,positive_count,negative_total,negative_count);
          }
        }
        else {
          if (!bCounts)
            printf("%10d %10d %10d\t%s\n",total,positive_total,negative_total,date_string);
          else {
            printf("%10d (%5d) %10d (%5d) %10d (%5d)\t%s\n",
              total,count,positive_total,positive_count,negative_total,negative_count,date_string);
          }
        }
      }
      else {
        if (!bHaveDateString) {
          if (!bCounts) {
            printf("%10d %10d %10d %s/%s\n",
              total,positive_total,negative_total,save_dir,argv[curr_arg]);
          }
          else {
            printf("%d (%d) %d (%d) %d (%d) %s/%s\n",
              total,count,positive_total,positive_count,negative_total,negative_count,save_dir,argv[curr_arg]);
          }
        }
        else {
          if (!bCounts) {
            printf("%d\t%s %d %d %s/%s\n",
              total,date_string,positive_total,negative_total,save_dir,argv[curr_arg]);
          }
          else {
            printf("%d (%d)\t%s %d (%d) %d (%d) %s/%s\n",
              total,count,date_string,positive_total,positive_count,negative_total,negative_count,save_dir,argv[curr_arg]);
          }
        }
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
