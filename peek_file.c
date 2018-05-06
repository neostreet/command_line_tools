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

static char usage[] = "usage: peek_file infile offset len\n";
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
  int fhndl;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  sscanf(argv[2],"%d",&offset);
  sscanf(argv[3],"%d",&len);

  bytes_to_read = (int)statbuf.st_size;

  if (offset + len > bytes_to_read) {
    printf("offset (%d) + len (%d) > bytes_to_read (%d)\n",
      offset,len,bytes_to_read);
    return 3;
  }

  if ((buf = (char *)malloc(bytes_to_read)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_read);
    return 4;
  }

  if ((fhndl = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 5;
  }

  bytes_read = read(fhndl,buf,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    printf("read of %d bytes failed\n",bytes_to_read);
    free(buf);
    close(fhndl);
    return 6;
  }

  buf[offset+len] = 0;
  printf("%s\n",&buf[offset]);

  free(buf);
  close(fhndl);

  return 0;
}
