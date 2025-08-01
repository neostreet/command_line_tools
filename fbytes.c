#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

static char usage[] =
"usage: fbytes (-debug) filename\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  bool bDebug;
  FILE *fptr;
  int line_no;
  int linelen;
  struct stat stat_buf;
  int total_bytes;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"-debug"))
      bDebug = true;
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
  total_bytes = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (stat(line,&stat_buf) != -1)
      total_bytes += stat_buf.st_size;
    else
      printf("stat() failed on %s\n",line);
  }

  fclose(fptr);

  printf("%d\n",total_bytes);

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
