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
"usage: freefall_from_the_peak (-debug) (-terse) (-verbose) (-boolean)\n"
"  (-pct_first) (-num_first) (-is_fftp) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_date_from_cwd(char *cwd,char **date_string_ptr);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bTerse;
  bool bVerbose;
  bool bBoolean;
  bool bPctFirst;
  bool bNumFirst;
  bool bIsFftp;
  FILE *fptr;
  int linelen;
  int line_no;
  int work;
  int total;
  int max;
  int max_ix;
  int last_pos_ix;
  double dwork;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bTerse = false;
  bVerbose = false;
  bBoolean = false;
  bPctFirst = false;
  bNumFirst = false;
  bIsFftp = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-boolean"))
      bBoolean = true;
    else if (!strcmp(argv[curr_arg],"-pct_first"))
      bPctFirst = true;
    else if (!strcmp(argv[curr_arg],"-num_first"))
      bNumFirst = true;
    else if (!strcmp(argv[curr_arg],"-is_fftp"))
      bIsFftp = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 3;
  }

  if (bPctFirst && bNumFirst) {
    printf("can't specify both -pct_first and -num_first\n");
    return 4;
  }

  getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  line_no = 0;

  total = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    sscanf(&line[0],"%d",&work);

    total += work;

    if ((line_no == 1) || (total > max)) {
      max = total;
      max_ix = line_no - 1;
    }

    if (work > 0)
      last_pos_ix = line_no - 1;
  }

  fclose(fptr);

  if (!bBoolean) {
    if (bIsFftp) {
      if (last_pos_ix == max_ix)
        work = 1;
      else
        work = 0;

      if (!bVerbose)
        printf("%d\n",work);
      else
        printf("%d %s\n",work,save_dir);
    }
    else if (last_pos_ix == max_ix) {
      dwork = (double)(line_no - (max_ix + 1)) / (double)line_no;

      if (bTerse)
        printf("%s\n",save_dir);
      else if (!bVerbose) {
        if (bPctFirst)
          printf("%lf %d (%d %d)\n",dwork,max,line_no - (max_ix + 1),line_no);
        else if (bNumFirst)
          printf("%d %lf %d (%d)\n",line_no - (max_ix + 1),dwork,max,line_no);
        else
          printf("%d (%d %d %lf)\n",max,line_no - (max_ix + 1),line_no,dwork);
      }
      else {
        if (bPctFirst)
          printf("%lf %d (%d %d) %s\n",dwork,max,line_no - (max_ix + 1),line_no,save_dir);
        else if (bNumFirst)
          printf("%d %lf %d (%d) %s\n",line_no - (max_ix + 1),dwork,max,line_no,save_dir);
        else
          printf("%d (%d %d %lf) %s\n",max,line_no - (max_ix + 1),line_no,dwork,save_dir);
      }
    }
  }
  else {
    if (!bVerbose)
      printf("%d\n",(last_pos_ix == max_ix));
    else
      printf("%d %s\n",(last_pos_ix == max_ix),save_dir);
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
