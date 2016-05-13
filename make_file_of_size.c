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

static char usage[] =
"usage: make_file_of_size size filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  off_t bytes_to_do;
  off_t bytes_per_chunk;
  off_t last_chunk_bytes;
  char *buf;
  int num_chunks;
  int fhndl;
  int bytes_read;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%lu",&bytes_to_do);

  if ((fhndl = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {

    printf(couldnt_open,argv[2]);
    return 2;
  }

  bytes_per_chunk = 1048576;

  if ((buf = (char *)malloc(bytes_per_chunk)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_per_chunk);
    return 3;
  }

  for (n = 0; n < bytes_per_chunk; n++)
    buf[n] = 0;

  num_chunks = (bytes_to_do + bytes_per_chunk - 1) / bytes_per_chunk;
  last_chunk_bytes = bytes_to_do - (num_chunks - 1) * bytes_per_chunk;

  for (n = 0; n < num_chunks; n++) {
    write(fhndl,buf,
      ((n < num_chunks - 1) ? bytes_per_chunk : last_chunk_bytes));
  }

  free(buf);

  close(fhndl);

  return 0;
}
