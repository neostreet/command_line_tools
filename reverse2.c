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

#define LINEFEED 0x0a

static char usage[] = "usage: reverse2 filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  off_t mem_amount;
  char *in_buf;
  int in_buf_ix;
  int fhndl;
  int bytes_to_io;
  int lines;
  char **cppt;
  int curr_line;

  if (argc != 2) {
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

  if ((fhndl = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(in_buf);
    return 4;
  }

  bytes_to_io = (int)mem_amount;

  if (read(fhndl,in_buf,bytes_to_io) != bytes_to_io) {
    printf(read_failed,argv[1],bytes_to_io);
    free(in_buf);
    close(fhndl);
    return 5;
  }

  lines = 0;

  for (n = 0; n < bytes_to_io; n++) {
    if (in_buf[n] == LINEFEED)
      lines++;
  }

  if ((cppt = (char **)malloc(lines * sizeof (char *))) == NULL) {
    printf(malloc_failed,lines * sizeof (char *));
    return 6;
  }

  curr_line = 0;
  cppt[curr_line++] = in_buf;

  for (n = 0; n < bytes_to_io; n++) {
    if (in_buf[n] == LINEFEED) {
      in_buf[n] = 0;

      if (curr_line < lines)
        cppt[curr_line++] = &in_buf[n + 1];
    }
  }

  for (n = lines - 1; n >= 0; n--)
    printf("%s\n",cppt[n]);

  free(cppt);
  free(in_buf);

  close(fhndl);

  return 0;
}
