#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr0;
  FILE *fptr;
  int line_len;
  int chara;
  int cur_l;
  int cur_lln;
  int max_l;
  int max_lln;
  int lines_of_max_len;
  int tot_lines;

  if (argc != 2) {
    printf("usage: maxllen filename\n");
    return 1;
  }

  if ((fptr0 = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  for ( ; ; ) {
    GetLine(fptr0,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(line,"r")) == NULL) {
      printf(couldnt_open,line);
      continue;
    }

    cur_l = 1;

    max_lln = 0;
    cur_lln = 0;
    lines_of_max_len = 0;
    tot_lines = 0;

    for ( ; ; ) {
      chara = fgetc(fptr);

      if (feof(fptr))
        break;

      if (chara == 0x0a) {
        if (cur_lln > max_lln) {
          max_lln = cur_lln;
          max_l = cur_l;
          lines_of_max_len = 1;
        }
        else if (cur_lln == max_lln)
          lines_of_max_len++;

        tot_lines++;

        cur_lln = 0;
        cur_l++;
      }
      else
        cur_lln++;
    }

    fclose(fptr);
    printf("%s: %d (%d, %d of %d total lines)\n",
      line,max_lln,max_l,lines_of_max_len,tot_lines);
  }

  fclose(fptr0);

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
