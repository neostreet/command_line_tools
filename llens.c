#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: llens (-verbose) filename (filename ...)\n";
static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  int linelen;
  int line_no;
  int num_files;
  int first_file_ix;
  int len;

  if (argc < 2) {
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

  num_files = argc - curr_arg;

  if (num_files < 1) {
    printf(usage);
    return 2;
  }

  first_file_ix = curr_arg;

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf("couldn't open %s\n",argv[curr_arg]);
      continue;
    }

    if (num_files > 1) {
      if (curr_arg > first_file_ix)
        putchar(0x0a);

      printf("%s\n",argv[curr_arg]);

      for (len = strlen(argv[curr_arg]); (len > 0); len--)
        putchar('=');

      putchar(0x0a);
    }

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (!bVerbose)
        printf("%d\n",linelen);
      else
        printf("%3d %s\n",linelen,line);
    }

    fclose(fptr);
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
