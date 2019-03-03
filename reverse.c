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

static char usage[] = "usage: reverse infile outfile\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";
static char write_failed[] = "%s: write of %d bytes failed\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  off_t mem_amount;
  char *in_buf;
  int in_buf_ix;
  char *out_buf;
  int out_buf_ix;
  int fhndl[2];
  int bytes_to_io;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  mem_amount = (size_t)statbuf.st_size;

  if ((in_buf = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    return 3;
  }

  if ((out_buf = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    free(in_buf);
    return 4;
  }

  if ((fhndl[0] = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(out_buf);
    free(in_buf);
    return 5;
  }

  if ((fhndl[1] = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);
    free(out_buf);
    free(in_buf);
    close(fhndl[0]);
    return 6;
  }

  bytes_to_io = (int)mem_amount;

  if (read(fhndl[0],in_buf,bytes_to_io) != bytes_to_io) {
    printf(read_failed,argv[1],bytes_to_io);
    free(out_buf);
    free(in_buf);
    close(fhndl[1]);
    close(fhndl[0]);
    return 7;
  }

  in_buf_ix = mem_amount - 1;
  out_buf_ix = 0;

  for (n = in_buf_ix; (n >= 0) && ((in_buf[n] == '\r') || (in_buf[n] == '\n')); n--)
    ;

  for ( ; ; ) {
    for ( ; ((n >= 0) && (in_buf[n] != '\r') && (in_buf[n] != '\n')); n--)
      ;

    memcpy(&out_buf[out_buf_ix],&in_buf[n+1],in_buf_ix - n);

    if (n < 0)
      break;

    out_buf_ix += in_buf_ix - n;

    in_buf_ix = n;

    for ( ; (n >= 0) && ((in_buf[n] == '\r') || (in_buf[n] == '\n')); n--)
      ;
  }

  if (write(fhndl[1],out_buf,bytes_to_io) != bytes_to_io) {
    printf(write_failed,argv[1],bytes_to_io);
    free(out_buf);
    free(in_buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 8;
  }

  free(out_buf);
  free(in_buf);

  for (n = 0; n < 2; n++)
    close(fhndl[n]);

  return 0;
}
