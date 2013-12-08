#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: cut_filter1 filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char temp_filename[] = "cut_filter1.tmp";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  FILE *ofptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  if ((ofptr = fopen(temp_filename,"w")) == NULL) {
    printf(couldnt_open,temp_filename);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no > 1)
      fprintf(ofptr,"%s\n",line);
  }

  fclose(ofptr);
  fclose(fptr);

  remove(argv[1]);
  rename(temp_filename,argv[1]);

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
