#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifndef CYGWIN
#define O_BINARY 0
#endif
#endif

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_FILENAME_LEN 128
static char output_filename[MAX_FILENAME_LEN+1];

#define TAB 0x09

static char usage[] = "usage: expand_grid num_rows num_cols filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void build_output_filename(char *file,int num_rows,int num_cols,char *output_filename);
static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int expand_line(char *line,int line_len,int line_no,int num_rows,int num_cols,char *grid);

int main(int argc,char **argv)
{
  int m;
  int n;
  int num_rows;
  int num_cols;
  int malloc_size;
  char *grid;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  int fhndl;
  unsigned int bytes_to_write;
  unsigned int bytes_written;

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

  build_output_filename(argv[3],num_rows,num_cols,output_filename);

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

  if ((fhndl = open(output_filename,
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,output_filename);
    free(grid);
    return 5;
  }

  bytes_to_write = malloc_size;
  bytes_written = write(fhndl,grid,bytes_to_write);

  if (bytes_written != bytes_to_write) {
    printf("failed to write %d bytes\n",bytes_to_write);
    close(fhndl);
    return 6;
  }

  close(fhndl);

  free(grid);

  return 0;
}

static void build_output_filename(char *file,int num_rows,int num_cols,char *output_filename)
{
  sprintf(output_filename,"%s.%d.%d.expand_grid",file,num_rows,num_cols);
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
      grid[offset + m] = 0;
      m++;
    }
    else {
      if (line[n] == 'X')
        grid[offset + m] = 0;
      else
        grid[offset + m] = 1;

      m++;
      n++;
    }
  }

  if (m < num_cols)
    grid[offset + m] = 0;

  return 0;
}
