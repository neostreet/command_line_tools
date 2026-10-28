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

static char usage[] = "usage: print_ints filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int read_ints_file(char *filename,int **ints_ptr,int *num_ints_ptr);

int main(int argc,char **argv)
{
  int n;
  int *ints;
  int num_ints;
  int retval;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = read_ints_file(argv[1],&ints,&num_ints);

  if (retval) {
    printf("read_ints_file() failed: %d\n",retval);
    return 2;
  }

  for (n = 0; n < num_ints; n++)
    printf("%d\n",ints[n]);

  free(ints);

  return 0;
}

int read_ints_file(char *filename,int **ints_ptr,int *num_ints_ptr)
{
  int m;
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  int *ints;
  int bytes_read;

  if (stat(filename,&statbuf) == -1) {
    printf(couldnt_get_status,filename);
    return 1;
  }

  bytes_to_io = (int)statbuf.st_size;

  if (bytes_to_io % sizeof(int)) {
    printf("the length of %s is not divisible by sizeof(int)\n",filename);
    return 2;
  }

  *num_ints_ptr = bytes_to_io / sizeof(int);

  if ((ints = (int *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 3;
  }

  if ((fhndl = open(filename,O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,filename);
    free(ints);
    return 4;
  }

  bytes_read = read(fhndl,ints,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(ints);
    close(fhndl);
    return 4;
  }

  close(fhndl);

  *ints_ptr = ints;

  return 0;
}
