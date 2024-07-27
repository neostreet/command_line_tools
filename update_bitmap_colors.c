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
#include <windows.h>

static char usage[] = "usage: update_bitmap_colors infile color1 color2 offset outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int color1;
  int color2;
  int offset;
  int *ipt;
  int out_fhndl;

  if (argc != 6) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  sscanf(argv[2],"%x",&color1);
  sscanf(argv[3],"%x",&color2);
  sscanf(argv[4],"%d",&offset);

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

  ipt = (int *)&buf[offset];
  ipt[0] = color1;
  ipt[1] = color2;

  if ((out_fhndl = open(argv[5],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[5]);

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
