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
#define O_BINARY 0
#endif

#define MAX_FILE_NAME_LEN 1024

#define BYTES_PER_LINE 16

static char usage[] = "usage: dump chunk_size filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

void do_dump(char *buf,off_t chunk_size,int offset);

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  off_t chunk_size;
  int num_chunks;
  off_t bytes_to_read;
  char *buf;
  int fhndl;
  int bytes_read;
  int offset;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&chunk_size);

  if (chunk_size % BYTES_PER_LINE) {
    printf("chunk_size must be a multiple of %d\n",BYTES_PER_LINE);
    return 2;
  }

  if (stat(argv[2],&statbuf) == -1) {
    printf(couldnt_get_status,argv[2]);
    return 3;
  }

  num_chunks = (statbuf.st_size + (chunk_size - 1)) / chunk_size;

  if ((buf = (char *)malloc(chunk_size)) == NULL) {
    printf("malloc of %d bytes failed\n",chunk_size);
    return 4;
  }

  if ((fhndl = open(argv[2],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[2]);
    free(buf);
    return 5;
  }

  offset = 0;

  for (n = 0; n < num_chunks; n++) {
    if (n != num_chunks - 1)
      bytes_to_read = chunk_size;
    else if (n)
      bytes_to_read = statbuf.st_size - (n * chunk_size);
    else
      bytes_to_read = statbuf.st_size;

    bytes_read = read(fhndl,buf,bytes_to_read);

    if (bytes_read != bytes_to_read) {
      printf("read of %d bytes failed\n",bytes_to_read);
      return 6;
    }

    do_dump(buf,bytes_read,offset);

    offset += bytes_read;
  }

  close(fhndl);

  free(buf);

  return 0;
}

void do_dump(char *buf,off_t chunk_size,int offset)
{
  int m;
  int n;
  int num_lines;
  int local_offset;
  int num_chars;
  int chara;
  int work;

  num_lines = (chunk_size + BYTES_PER_LINE - 1) / BYTES_PER_LINE;
  local_offset = 0;

  for (n = 0; n < num_lines; n++) {
    if (n < num_lines - 1)
      num_chars = BYTES_PER_LINE;
    else
      num_chars = chunk_size - local_offset;

    printf("%08x   ",offset);

    for (m = 0; m < num_chars; m++) {
      work = buf[local_offset+m];
      work &= 0xff;

      printf("%02x",work);

      if (m == 7)
        putchar('-');
      else
        putchar(' ');
    }

    for ( ; m < BYTES_PER_LINE; m++)
      printf("   ");

    printf("  ");

    for (m = 0; m < num_chars; m++) {
      chara = buf[local_offset+m];

      if ((chara >= ' ') && (chara < 0x7f))
        putchar(chara);
      else
        putchar('.');
    }

    putchar(0x0a);

    offset += BYTES_PER_LINE;
    local_offset += BYTES_PER_LINE;
  }
}
