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
"usage: make_file_of_size (-zero) size filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bZero;
  unsigned int to_do;
  off_t bytes_to_do;
  unsigned int per_chunk;
  off_t bytes_per_chunk;
  unsigned int last_chunk;
  off_t last_chunk_bytes;
  char *buf;
  int num_chunks;
  int fhndl;
  int bytes_read;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bZero = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-zero"))
      bZero = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%u",&to_do);
  bytes_to_do = (off_t)to_do;

  if ((fhndl = open(argv[curr_arg+1],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {

    printf(couldnt_open,argv[curr_arg+1]);
    return 3;
  }

  per_chunk = 1048576;
  bytes_per_chunk = (off_t)per_chunk;

  if ((buf = (char *)malloc(per_chunk)) == NULL) {
    printf("malloc of %d bytes failed\n",per_chunk);
    return 4;
  }

  for (n = 0; n < per_chunk; n++) {
    if (bZero)
      buf[n] = 0;
    else
      buf[n] = 'a' + (n % 26);
  }

  num_chunks = (to_do + per_chunk - 1) / per_chunk;
  last_chunk = to_do - (num_chunks - 1) * per_chunk;
  last_chunk_bytes = (off_t)last_chunk;

  for (n = 0; n < num_chunks; n++) {
    write(fhndl,buf,
      ((n < num_chunks - 1) ? bytes_per_chunk : last_chunk_bytes));
  }

  free(buf);

  close(fhndl);

  return 0;
}
