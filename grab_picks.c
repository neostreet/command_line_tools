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

static char usage[] = "usage: grab_picks num_rows num_cols col filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static void build_output_filename(char *file,int col,char *output_filename);

int main(int argc,char **argv)
{
  int n;
  int num_rows;
  int num_cols;
  int col;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int line_len;
  FILE *out_fptr;
  int total_picks;
  int correct_picks;
  double percentage;

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&num_rows);
  sscanf(argv[2],"%d",&num_cols);
  sscanf(argv[3],"%d",&col);

  if (col >= num_cols) {
    printf("invalid column\n");
    return 2;
  }

  if (stat(argv[4],&statbuf) == -1) {
    printf(couldnt_get_status,argv[4]);
    return 3;
  }

  bytes_to_io = (int)statbuf.st_size;

  if (bytes_to_io != num_rows * num_cols) {
    printf("%s is the wrong size\n",argv[4]);
    return 4;
  }

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 5;
  }

  if ((fhndl = open(argv[4],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[4]);
    free(buf);
    return 6;
  }

  bytes_read = read(fhndl,buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl);
    return 7;
  }

  build_output_filename(argv[4],col,output_filename);

  if ((out_fptr = fopen(output_filename,"w")) == NULL) {
    printf(couldnt_open,output_filename);
    return 8;
  }

  total_picks = num_rows / 2;
  correct_picks = 0;

  for (n = 0; n < num_rows; n++) {
    if (buf[(n * num_cols) + col])
      correct_picks++;
  }

  percentage = (double)correct_picks / (double)total_picks * (double)100;
  fprintf(out_fptr,"%d of %d, %6.2lf%%\n",correct_picks,total_picks,percentage);

  free(buf);
  close(fhndl);

  fclose(out_fptr);

  return 0;
}

static void build_output_filename(char *file,int col,char *output_filename)
{
  sprintf(output_filename,"%s.%d.grab_picks",file,col);
}
