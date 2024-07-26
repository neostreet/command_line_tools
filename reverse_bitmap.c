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

static char usage[] = "usage: reverse_bitmap infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int swap_nibbles(int chara);

int main(int argc,char **argv)
{
  int m;
  int n;
  char chara1;
  char chara2;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int offset;
  int out_fhndl;

  if (argc != 3) {
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
    return 5;
  }

  for (m = 0; m < 200; m++) {
    offset = 118 + m * ((50 * 13) + 2);

    for (n = 0; n < 25 * 13; n++) {
       chara1 = (char)swap_nibbles(buf[offset+n]);
       chara2 = (char)swap_nibbles(buf[offset + (50 * 13) - (1 + n)]);

       buf[offset+n] = chara2;
       buf[offset + (50 * 13) - (1 + n)] = chara1;
    }
  }

  if ((out_fhndl = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);

    free(buf);
    close(fhndl);
    return 6;
  }

  if (write(out_fhndl,buf,bytes_to_io) != bytes_to_io) {
    printf("write failed\n");

    free(buf);
    close(fhndl);
    close(out_fhndl);

    return 7;
  }

  free(buf);
  close(fhndl);
  close(out_fhndl);

  return 0;
}

int swap_nibbles(int chara)
{
  int upper_nibble;
  int lower_nibble;
  int swapped;

  upper_nibble = (chara & 0xf0) >> 4;
  lower_nibble = (chara & 0x0f);

  swapped = (lower_nibble << 4) | upper_nibble;

  return swapped;
}
