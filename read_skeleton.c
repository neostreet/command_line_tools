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

static char usage[] = "usage: read_skeleton filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int read_list_file(char *filename,char **buf_ptr,int *num_lines_ptr,int **offsets_ptr);

int main(int argc,char **argv)
{
  int n;
  char *buf;
  int num_lines;
  int *offsets;
  int retval;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  retval = read_list_file(argv[1],&buf,&num_lines,&offsets);

  if (retval) {
    printf("read_list_file() failed: %d\n",retval);
    return 2;
  }

  for (n = 0; n < num_lines; n++)
    printf("%s\n",&buf[offsets[n]]);

  free(offsets);
  free(buf);

  return 0;
}

int read_list_file(char *filename,char **buf_ptr,int *num_lines_ptr,int **offsets_ptr)
{
  int m;
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int num_lines;
  int malloc_size;
  int *offsets;

  if (stat(filename,&statbuf) == -1) {
    printf(couldnt_get_status,filename);
    return 1;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 2;
  }

  if ((fhndl = open(filename,O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,filename);
    free(buf);
    return 3;
  }

  bytes_read = read(fhndl,buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl);
    return 4;
  }

  num_lines = 0;

  for (n = 0; n < bytes_to_io; n++) {
    if (buf[n] == 0x0a) {
      num_lines++;
      buf[n] = 0;
    }
  }

  malloc_size = sizeof (int) * num_lines;

  if ((offsets = (int *)malloc(malloc_size)) == NULL) {
    printf("malloc of %d bytes failed\n",malloc_size);
    return 5;
  }

  m = 0;
  offsets[0] = 0;

  for (n = 1; n < num_lines; n++) {
    for ( ; m < bytes_to_io; m++) {
      if (!buf[m]) {
        m++;
        break;
      }
    }

    offsets[n] = m;
  }

  close(fhndl);

  *buf_ptr = buf;
  *num_lines_ptr = num_lines;
  *offsets_ptr = offsets;

  return 0;
}
