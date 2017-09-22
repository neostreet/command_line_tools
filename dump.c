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
#ifndef CYGWIN
#define O_BINARY 0
#endif
#endif

#define MAX_FILE_NAME_LEN 1024

#define BYTES_PER_LINE 16

static char usage[] =
"usage: dump (-max_chunksn) (-skip_ascii) chunk_size filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

void do_dump(char *buf,off_t chunk_size,int offset,bool bSkipAscii);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int max_chunks;
  bool bSkipAscii;
  struct stat statbuf;
  int chunk_size;
  int num_chunks;
  off_t bytes_to_read;
  char *buf;
  int fhndl;
  int bytes_read;
  int offset;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  max_chunks = -1;
  bSkipAscii = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-max_chunks",11))
      sscanf(&argv[curr_arg][11],"%d",&max_chunks);
    else if (!strcmp(argv[curr_arg],"-skip_ascii"))
      bSkipAscii = true;
    else
     break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&chunk_size);

  if (chunk_size % BYTES_PER_LINE) {
    printf("chunk_size must be a multiple of %d\n",BYTES_PER_LINE);
    return 3;
  }

  if (stat(argv[curr_arg+1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg+1]);
    return 4;
  }

  num_chunks = (statbuf.st_size + (chunk_size - 1)) / chunk_size;

  if ((buf = (char *)malloc(chunk_size)) == NULL) {
    printf("malloc of %d bytes failed\n",chunk_size);
    return 5;
  }

  if ((fhndl = open(argv[curr_arg+1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg+1]);
    free(buf);
    return 6;
  }

  offset = 0;

  for (n = 0; n < num_chunks; ) {
    if (n != num_chunks - 1)
      bytes_to_read = chunk_size;
    else if (n)
      bytes_to_read = statbuf.st_size - (n * chunk_size);
    else
      bytes_to_read = statbuf.st_size;

    bytes_read = read(fhndl,buf,bytes_to_read);

    if (bytes_read != bytes_to_read) {
      printf("read of %d bytes failed\n",bytes_to_read);
      return 7;
    }

    do_dump(buf,bytes_read,offset,bSkipAscii);

    n++;

    if ((max_chunks != -1) && (n == max_chunks))
      break;

    offset += bytes_read;
  }

  close(fhndl);

  free(buf);

  return 0;
}

void do_dump(char *buf,off_t chunk_size,int offset,bool bSkipAscii)
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

    if (!bSkipAscii) {
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
    }

    putchar(0x0a);

    offset += BYTES_PER_LINE;
    local_offset += BYTES_PER_LINE;
  }
}
