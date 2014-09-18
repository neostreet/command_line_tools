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

static char usage[] = "usage: ptch infile1 infile2 offset outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  int fhndl[2];
  int bytes_to_io[2];
  char *buf[2];
  int bytes_read;
  int offset;
  int out_fhndl;

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  sscanf(argv[3],"%x",&offset);

  if (!strcmp(argv[1],argv[4])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  for (n = 0; n < 2; n++) {
    if (stat(argv[1+n],&statbuf) == -1) {
      printf(couldnt_get_status,argv[1+n]);
      return 3;
    }

    bytes_to_io[n] = (int)statbuf.st_size;

    if ((buf[n] = (char *)malloc(bytes_to_io[n])) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_io[n]);
      return 4;
    }

    if ((fhndl[n] = open(argv[1+n],O_BINARY | O_RDONLY,0)) == -1) {
      printf(couldnt_open,argv[1+n]);
      free(buf[n]);
      return 5;
    }

    bytes_read = read(fhndl[n],buf[n],bytes_to_io[n]);

    if (bytes_read != bytes_to_io[n]) {
      printf("read of %d bytes failed\n",bytes_to_io);
      free(buf[n]);
      close(fhndl[n]);
      return 6;
    }
  }

  for (n = 0; n < bytes_to_io[1]; n++)
    buf[0][offset+n] = buf[1][n];

  if ((out_fhndl = open(argv[4],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[4]);

    for (n = 0; n < 2; n++)
      free(buf[n]);

    close(fhndl[0]);
    return 7;
  }

  if (write(out_fhndl,buf[0],bytes_to_io[0]) != bytes_to_io[0]) {
    printf("write failed\n");

    for (n = 0; n < 2; n++) {
      free(buf[n]);
      close(fhndl[n]);
    }

    close(out_fhndl);

    return 8;
  }

  for (n = 0; n < 2; n++) {
    free(buf[n]);
    close(fhndl[n]);
  }

  close(out_fhndl);

  return 0;
}
