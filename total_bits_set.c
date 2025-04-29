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

static char usage[] = "usage: total_bits_set filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int and_val;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int total_bits_set;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 3;
  }

  if ((fhndl = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 4;
  }

  bytes_read = read(fhndl,buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl);
    return 6;
  }

  total_bits_set = 0;

  for (n = 0; n < bytes_to_io; n++) {
    for (m = 0; m < 8; m++) {
      if (!m)
        and_val = 1;
      else
        and_val <<= 1;

      if (buf[n] & and_val)
        total_bits_set++;
    }
  }

  free(buf);
  close(fhndl);

  printf("%d\n",total_bits_set);

  return 0;
}
