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

static char usage[] = "usage: add_crlf infile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static const char *tempname = "add_crlf.tmp";

int main(int argc,char **argv)
{
  struct stat statbuf;
  int bytes_to_read;
  int bytes_read;
  char *buf;
  int fhndl[2];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  bytes_to_read = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_read+2)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_read+2);
    return 3;
  }

  if ((fhndl[0] = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 4;
  }

  if ((fhndl[1] = open((const char *)tempname,
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,tempname);
    free(buf);
    close(fhndl[0]);
    return 5;
  }

  bytes_read = read(fhndl[0],buf,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    printf("read of %d bytes failed\n",bytes_to_read);
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 6;
  }

  buf[bytes_to_read] = 0x0d;
  buf[bytes_to_read+1] = 0x0a;

  if (write(fhndl[1],buf,bytes_to_read+2) != bytes_to_read+2) {
    printf("write failed\n");
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 7;
  }

  free(buf);
  close(fhndl[0]);
  close(fhndl[1]);

  remove(argv[1]);
  rename(tempname,argv[1]);

  return 0;
}
