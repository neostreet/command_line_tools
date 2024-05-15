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

static char usage[] = "usage: build_grab5 filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int offset;

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

  printf("0\n0\n1300\n50\n");

  for (m = 0; m < 50; m++) {
    offset = 118 + (49 - m) * ((50 * 13) + 2);

    for (n = 0; n < 50 * 13; n++) {
      if (buf[offset + n] == 0x66) {
        printf("abd0eb\n");
        printf("abd0eb\n");
      }
      else if (buf[offset + n] == 0x67) {
        printf("abd0eb\n");
        printf("2e81be\n");
      }
      else if (buf[offset + n] == 0x76) {
        printf("2e81be\n");
        printf("abd0eb\n");
      }
      else if (buf[offset + n] == 0x77) {
        printf("2e81be\n");
        printf("2e81be\n");
      }
    }
  }

  free(buf);
  close(fhndl);

  return 0;
}
