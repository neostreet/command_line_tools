#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define TAB 0x09

static char usage[] = "usage: expand_grid num_rows num_cols filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int expand_line(char *line,int line_len,int line_no,int num_rows,int num_cols,char *grid);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int num_rows;
  int num_cols;
  int malloc_size;
  char *grid;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&num_rows);
  sscanf(argv[2],"%d",&num_cols);

  malloc_size = num_rows * num_cols;

  if ((grid = (char *)malloc(malloc_size)) == NULL) {
    printf("malloc of %d bytes failed\n",malloc_size);
    return 2;
  }

  if ((fptr = fopen(argv[3],"r")) == NULL) {
    printf(couldnt_open,argv[3]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    retval = expand_line(line,line_len,line_no,num_rows,num_cols,grid);

    line_no++;

    if (retval) {
      printf("expand_line() failed on line %d: %d\n",line_no,retval);
      return 4;
    }
  }

  fclose(fptr);

  p = 0;

  for (m = 0; m < num_rows; m++) {
    for (n = 0; n < num_cols; n++)
      putchar(grid[p++]);

     putchar(0x0a);
  }

  free(grid);

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

static int expand_line(char *line,int line_len,int line_no,int num_rows,int num_cols,char *grid)
{
  int m;
  int n;
  int offset;

  if (line_no == num_rows)
    return 1;

  offset = line_no * num_cols;
  m = 0;

  for (n = 0; n < line_len; n++) {
    if (m == num_cols)
      return 2;

    if (line[n] == TAB) {
      grid[offset + m] = ' ';
      m++;
    }
    else {
      grid[offset + m] = line[n];
      m++;
      n++;
    }
  }

  if (m < num_cols)
    grid[offset + m] = ' ';

  return 0;
}
