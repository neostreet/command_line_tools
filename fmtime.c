#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

static char usage[] =
"usage: fmtime (-noctime) (-suppress_errors) (-size_only) filename\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  bool bNoCtime;
  bool bSuppressErrors;
  bool bSizeOnly;
  FILE *fptr;
  int line_no;
  int linelen;
  struct stat stat_buf;
  char *cpt;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bNoCtime = false;
  bSuppressErrors = false;
  bSizeOnly = false;

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"-noctime"))
      bNoCtime = true;
    else if (!strcmp(argv[n],"-suppress_errors"))
      bSuppressErrors = true;
    else if (!strcmp(argv[n],"-size_only"))
      bSizeOnly = true;
    else
      break;
  }

  if (argc - n != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[n],"r")) == NULL) {
    printf("couldn't open %s\n",argv[n]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (stat(line,&stat_buf) != -1) {
      if (bSizeOnly)
        printf("%10d %s\n",stat_buf.st_size,line);
      else if (bNoCtime)
        printf("%10d %s\n",stat_buf.st_mtime,line);
      else {
        cpt = ctime(&stat_buf.st_mtime);
        cpt[strlen(cpt) - 1] = 0;
        printf("%s %10d ",cpt,stat_buf.st_size);
        printf("%s\n",line);
      }
    }
    else {
      if (!bSuppressErrors)
        printf("stat() failed on %s\n",line);
    }
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
