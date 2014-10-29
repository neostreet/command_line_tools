#include <stdio.h>
#include <string.h>

#define TAB 0x09

static char usage[] =
"usage: delim_count (-detail) (-verbose) delim filename (filename ...)\n";

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int count_delims(char *line,int linel,int delim,int line_no,bool bDetail,bool bVerbose);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDetail;
  bool bVerbose;
  int delim;
  FILE *fptr;
  int linel;
  int line_no;
  int delim_count;

  if (argc < 3) {
    printf(usage);
    return 1;
  }

  bDetail = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-detail"))
      bDetail = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg],"tab"))
    delim = TAB;
  else
    delim = argv[curr_arg][0];

  curr_arg++;

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf("couldn't open %s\n",argv[curr_arg]);
      continue;
    }

    if (bDetail) {
      printf("%s\n",argv[curr_arg]);

      for (n = strlen(argv[curr_arg]) - 1; (n >= 0); n--)
        putchar('=');

      putchar(0x0a);
    }

    line_no = 0;
    delim_count = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linel,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      delim_count += count_delims(line,linel,delim,line_no,bDetail,bVerbose);
    }

    fclose(fptr);

    if (!bDetail)
      printf("%5d %s\n",delim_count,argv[curr_arg]);
    else {
      printf("============\n");
      printf("       %5d\n\n",delim_count);
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

int count_delims(char *line,int linel,int delim,int line_no,bool bDetail,bool bVerbose)
{
  int n;
  int delims;

  delims = 0;

  for (n = 0; n < linel; n++) {
    if (line[n] == delim) {
      delims++;

      if (bVerbose)
        printf("%d %d (%x)\n",delims,n,n);

      for ( ; n < linel; n++) {
        if (line[n] != delim)
          break;
      }

      if (n == linel)
        break;

      n--;
    }
  }

  if (bDetail)
    printf("%5d: %5d\n",line_no,delims);

  return delims;
}
