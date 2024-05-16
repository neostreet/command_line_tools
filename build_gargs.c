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

#define INITIAL_OFFSET 118
#define PIECE_WIDTH_IN_CHARS (50 / 2)
#define ROW_WIDTH_IN_CHARS (PIECE_WIDTH_IN_CHARS * 26 + 2)
#define KNIGHT_BITMAPS_OFFSET_IN_ROW (PIECE_WIDTH_IN_CHARS * 8)
#define KNIGHT_BITMAPS_IN_ROW 4
#define KNIGHT_BITMAPS_WIDTH_IN_CHARS (PIECE_WIDTH_IN_CHARS * KNIGHT_BITMAPS_IN_ROW)
#define PIECE_HEIGHT 50

static char usage[] = "usage: build_gargs infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int swap_nibbles(int chara);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  char chara1;
  char chara2;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int initial_from_offset;
  int initial_to_offset;
  int from_offset;
  int to_offset;
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

  // copy non-highlighted knight pieces to non-highlighted gargantua pieces and reverse them

  initial_to_offset = INITIAL_OFFSET + KNIGHT_BITMAPS_OFFSET_IN_ROW + ROW_WIDTH_IN_CHARS * PIECE_HEIGHT;
  initial_from_offset = initial_to_offset + ROW_WIDTH_IN_CHARS * PIECE_HEIGHT * 2;
  initial_to_offset += PIECE_WIDTH_IN_CHARS - 1;

  for (p = 0; p < KNIGHT_BITMAPS_IN_ROW; p++) {
    to_offset = initial_to_offset + p * PIECE_WIDTH_IN_CHARS;
    from_offset = initial_from_offset + p * PIECE_WIDTH_IN_CHARS;

    for (m = 0; m < PIECE_HEIGHT; m++) {
      for (n = 0; n < PIECE_WIDTH_IN_CHARS; n++) {
        chara1 = buf[from_offset + n];
        chara2 = (char)swap_nibbles(chara1);
        buf[to_offset - n] = chara2;
      }

      from_offset += ROW_WIDTH_IN_CHARS;
      to_offset += ROW_WIDTH_IN_CHARS;
    }
  }

  // copy highlighted knight pieces to highlighted gargantua pieces and reverse thm

  initial_to_offset = INITIAL_OFFSET + KNIGHT_BITMAPS_OFFSET_IN_ROW;
  initial_from_offset = initial_to_offset + ROW_WIDTH_IN_CHARS * PIECE_HEIGHT * 2;
  initial_to_offset += PIECE_WIDTH_IN_CHARS - 1;

  for (p = 0; p < KNIGHT_BITMAPS_IN_ROW; p++) {
    to_offset = initial_to_offset + p * PIECE_WIDTH_IN_CHARS;
    from_offset = initial_from_offset + p * PIECE_WIDTH_IN_CHARS;

    for (m = 0; m < PIECE_HEIGHT; m++) {
      for (n = 0; n < PIECE_WIDTH_IN_CHARS; n++) {
        chara1 = buf[from_offset + n];
        chara2 = (char)swap_nibbles(chara1);
        buf[to_offset - n] = chara2;
      }

      from_offset += ROW_WIDTH_IN_CHARS;
      to_offset += ROW_WIDTH_IN_CHARS;
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
