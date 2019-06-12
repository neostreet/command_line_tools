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

static char usage[] =
"usage: last (-verbose) num_lines filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  int num_last_lines;
  FILE *fptr;
  int linelen;
  int line_no;
  int num_lines;
  int num_files;

  if (argc < 3) {
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

  if (argc - curr_arg < 2) {
    printf(usage);
    return 1;
  }

  if (bVerbose)
    getcwd(save_dir,_MAX_PATH);

  sscanf(argv[curr_arg++],"%d",&num_last_lines);
  num_files = argc - curr_arg;

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    if (!bVerbose && (num_files > 1))
      printf("%s\n",argv[curr_arg]);

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;
    }

    num_lines = line_no;

    fseek(fptr,0L,SEEK_SET);

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no >= (num_lines - (num_last_lines - 1))) {
        if (!bVerbose)
          printf("%s\n",line);
        else
          printf("%s %s\n",line,save_dir);
      }
    }

    if (num_files > 1)
      putchar(0x0a);

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
