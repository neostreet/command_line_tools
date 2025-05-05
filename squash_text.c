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
#include "bitfuns.h"

static char usage[] = "usage: squash_text infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  int bytes_to_io[2];
  int bytes_iod[2];
  unsigned char *buf[2];
  int fhndl[2];
  unsigned int seven_bits;
  unsigned short num_bits[2];
  unsigned int bit_offset[2];
  unsigned short bits;

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

  bytes_to_io[0] = (int)statbuf.st_size;
  bytes_to_io[1] = bytes_to_io[0];

  for (n = 0; n < 2; n++) {
    if ((buf[n] = (unsigned char *)malloc(bytes_to_io[n])) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_io[n]);
      return 4;
    }
  }

  if ((fhndl[0] = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);

    for (n = 0; n < 2; n++)
      free(buf[n]);

    return 5;
  }

  if ((fhndl[1] = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);

    for (n = 0; n < 2; n++)
      free(buf[n]);

    close(fhndl[0]);

    return 6;
  }

  bytes_iod[0] = read(fhndl[0],buf[0],bytes_to_io[0]);

  if (bytes_iod[0] != bytes_to_io[0]) {
    printf("read of %d bytes failed\n",bytes_to_io[0]);

    for (n = 0; n < 2; n++)
      free(buf[n]);

    close(fhndl[0]);
    close(fhndl[1]);

    return 7;
  }

  num_bits[0] = 8;
  num_bits[1] = 7;
  bit_offset[0] = 0;
  bit_offset[1] = 0;

  for (n = 0; n < bytes_iod[0]; n++) {
    bits = get_bits(num_bits[0],buf[0],bit_offset[0]);
    set_bits(num_bits[1],buf[1],bit_offset[1],bits);
    bit_offset[0] += num_bits[0];
    bit_offset[1] += num_bits[1];
  }

  seven_bits = bytes_iod[0] * 7;
  bytes_to_io[1] = seven_bits / 8;

  if (write(fhndl[1],buf[1],bytes_to_io[1]) != bytes_to_io[1]) {
    printf("write of %d bytes failed\n",bytes_to_io[1]);

    for (n = 0; n < 2; n++)
      free(buf[n]);

    close(fhndl[0]);
    close(fhndl[1]);

    return 6;
  }

  for (n = 0; n < 2; n++)
    free(buf[n]);

  close(fhndl[0]);
  close(fhndl[1]);

  return 0;
}
