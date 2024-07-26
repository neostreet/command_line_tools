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

static char usage[] = "usage: parse_bitmap filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  BITMAPFILEHEADER *bmfh_pt;
  BITMAPINFOHEADER *bmih_pt;

  if (argc != 2) {
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
    return 6;
  }

  bmfh_pt = (BITMAPFILEHEADER *)buf;
  bmih_pt = (BITMAPINFOHEADER *)&buf[sizeof (BITMAPFILEHEADER)];

  printf("sizeof (BITMAPFILEHEADER) = %d %x\n",sizeof (BITMAPFILEHEADER),sizeof (BITMAPFILEHEADER));
  printf("sizeof (BITMAPINFOHEADER) = %d %x\n\n",sizeof (BITMAPINFOHEADER),sizeof (BITMAPINFOHEADER));
  printf("sizeof (WORD) = %d %x\n",sizeof (WORD),sizeof (WORD));
  printf("sizeof (DWORD) = %d %x\n\n",sizeof (DWORD),sizeof (DWORD));

  printf("bfType = %c%c\n",buf[0],buf[1]);
  printf("bfSize = %ld %lx\n",bmfh_pt->bfSize,bmfh_pt->bfSize);
  printf("bfOffBits = %ld %lx\n",bmfh_pt->bfOffBits,bmfh_pt->bfOffBits);
  printf("biSize = %ld %lx\n",bmih_pt->biSize,bmih_pt->biSize);
  printf("biWidth = %ld %lx\n",bmih_pt->biWidth,bmih_pt->biWidth);
  printf("biHeight = %ld %lx\n",bmih_pt->biHeight,bmih_pt->biHeight);
  printf("biBitCount = %d %x\n",bmih_pt->biBitCount,bmih_pt->biBitCount);
  printf("biCompression = %ld %lx\n",bmih_pt->biCompression,bmih_pt->biCompression);
  printf("biClrUsed = %ld %lx\n",bmih_pt->biClrUsed,bmih_pt->biClrUsed);

  free(buf);
  close(fhndl);

  return 0;
}
