#include <stdio.h>

#define RETURN   0x0d
#define LINEFEED 0x0a

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: unfixtxt filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";
static char tempname[] = "unfixtxt.tmp";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  FILE *fptr1;
  FILE *fptr2;
  int linelen;
  int line_no;
  int bFix;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    bFix = 1;

    if ((fptr1 = fopen(argv[curr_arg],"rb")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    if ((fptr2 = fopen(tempname,"wb")) == NULL) {
      fclose(fptr1);
      printf(couldnt_open,tempname);
      continue;
    }

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr1,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr1))
        break;

      line_no++;

      if (line[linelen-1] == RETURN) {
        line[linelen-1] = LINEFEED;
        fprintf(fptr2,"%s",line);
      }
      else {
        bFix = 0;
        break;
      }
    }

    fclose(fptr1);
    fclose(fptr2);

    if (bFix) {
      remove(argv[curr_arg]);
      rename(tempname,argv[curr_arg]);
    }
    else
      remove(tempname);
  }

  return 0;
}

static void GetLine(FILE *fptr1,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr1);

    if (feof(fptr1))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}
