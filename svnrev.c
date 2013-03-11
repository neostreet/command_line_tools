#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static char usage[] = "svnrev (-verbose) filename\n";
static char tmpname[] = "svnrev.tmp";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

#define BUF_LEN 4096
static char buf[BUF_LEN];

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  int line_len;
  int rev;

  if ((argc != 2) && (argc != 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  sprintf(buf,"svn log %s > %s",argv[curr_arg],tmpname);
  system(buf);

  if ((fptr = fopen(tmpname,"r")) == NULL) {
    printf(couldnt_open,tmpname);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if ((line_len >= 10) && (line[0] == 'r') &&
      (line[1] >= '1') && (line[1] <= '9')) {

      for (n = 1; n < line_len; n++) {
        if (line[n] == ' ') {
          line[n] = 0;
          sscanf(&line[1],"%d",&rev);
          break;
        }
      }

      if (!bVerbose)
        printf("r%d\n",rev);
      else
        printf("%s.r%d\n",argv[curr_arg],rev);
    }
  }

  fclose(fptr);

  remove(tmpname);

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
