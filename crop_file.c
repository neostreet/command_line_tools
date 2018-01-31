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

static char usage[] = "usage: crop_file infile offset len outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  struct stat statbuf;
  int offset;
  int len;
  int bytes_to_read;
  int bytes_read;
  char *buf;
  int fhndl[2];

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],argv[4])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 3;
  }

  sscanf(argv[2],"%d",&offset);
  sscanf(argv[3],"%d",&len);

  bytes_to_read = (int)statbuf.st_size;

  if (offset + len > bytes_to_read) {
    printf("offset (%d) + len (%d) > bytes_to_read (%d)\n",
      offset,len,bytes_to_read);
    return 4;
  }

  if ((buf = (char *)malloc(bytes_to_read)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_read);
    return 5;
  }

  if ((fhndl[0] = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 6;
  }

  if ((fhndl[1] = open(argv[4],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[4]);
    free(buf);
    close(fhndl[0]);
    return 7;
  }

  bytes_read = read(fhndl[0],buf,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    printf("read of %d bytes failed\n",bytes_to_read);
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 8;
  }

  if (write(fhndl[1],&buf[offset],len) != len) {
    printf("write failed\n");
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 9;
  }

  free(buf);
  close(fhndl[0]);
  close(fhndl[1]);

  return 0;
}
