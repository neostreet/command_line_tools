#include <stdio.h>

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: last num_lines filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
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

  sscanf(argv[1],"%d",&num_last_lines);

  num_files = argc - 2;

  for (n = 2; n < argc; n++) {
    if ((fptr = fopen(argv[n],"r")) == NULL) {
      printf(couldnt_open,argv[n]);
      continue;
    }

    if (num_files > 1)
      printf("%s\n",argv[n]);

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

      if (line_no >= (num_lines - (num_last_lines - 1)))
        printf("%s\n",line);
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
