#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define TAB 0x09

static char usage[] = "usage: expand_cells num_cells filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static void expand_line(char *line,int line_len,int num_cells);

int main(int argc,char **argv)
{
  int num_cells;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&num_cells);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    expand_line(line,line_len,num_cells);
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

static void expand_line(char *line,int line_len,int num_cells)
{
  int n;
  int count;

  count = 0;

  for (n = 0; n < line_len; n++) {
    if (line[n] == TAB) {
      putchar(' ');
      count++;
    }
    else {
      putchar(line[n]);
      count++;
      n++;
    }
  }

  if (count < num_cells)
    putchar(' ');

  putchar(0x0a);
}
