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
#ifdef FREEBSD
#define O_BINARY 0
#endif
#endif

static char usage[] = "usage: file_size size outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  struct stat statbuf;
  int bytes_to_io;
  int bytes_read;
  char *buf;
  int fhndl;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&bytes_to_io);

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 2;
  }

  if ((fhndl = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);
    free(buf);
    return 3;
  }

  if (write(fhndl,buf,bytes_to_io) != bytes_to_io) {
    printf("write failed\n");
    free(buf);
    close(fhndl);
    return 4;
  }

  free(buf);
  close(fhndl);

  return 0;
}
