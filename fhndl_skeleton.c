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
#endif

static char usage[] = "usage: fhndl_skeleton infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  struct stat statbuf;
  int bytes_to_io;
  int bytes_read;
  char *buf;
  int fhndl[2];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],argv[2])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 3;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 4;
  }

  if ((fhndl[0] = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 5;
  }

  if ((fhndl[1] = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);
    free(buf);
    close(fhndl[0]);
    return 6;
  }

  bytes_read = read(fhndl[0],buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 7;
  }

  if (write(fhndl[1],buf,bytes_to_io) != bytes_to_io) {
    printf("write failed\n");
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 6;
  }

  free(buf);
  close(fhndl[0]);
  close(fhndl[1]);

  return 0;
}
