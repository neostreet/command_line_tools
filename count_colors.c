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

static char usage[] = "usage: count_colors filename\n";
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
  int count_66;
  int count_67;
  int count_76;
  int count_77;
  int bytes_scanned;
  int total;
  int contig_count;
  int num_contigs;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  bytes_to_io = (int)statbuf.st_size;
  printf("bytes_to_io = %d\n",bytes_to_io);

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

  count_66 = 0;
  count_67 = 0;
  count_76 = 0;
  count_77 = 0;
  contig_count = 0;
  num_contigs = 0;

  for (n = 118; n < bytes_read; n++) {
    if (buf[n] == 0x66) {
      count_66++;
      contig_count++;
    }
    else if (buf[n] == 0x67) {
      count_67++;
      contig_count++;
    }
    else if (buf[n] == 0x76) {
      count_76++;
      contig_count++;
    }
    else if (buf[n] == 0x77) {
      count_77++;
      contig_count++;
    }
    else if (!buf[n]) {
      if (contig_count) {
        printf("contig_count = %d\n",contig_count);
        contig_count = 0;
        num_contigs++;
      }
    }
    else
      break;
  }

  printf("num_contigs = %d\n",num_contigs);
  bytes_scanned = n - 118;
  printf("bytes scanned = %d\n",bytes_scanned);
  printf("count_66 = %d\n",count_66);
  printf("count_67 = %d\n",count_67);
  printf("count_76 = %d\n",count_76);
  printf("count_77 = %d\n",count_77);
  total = count_66 + count_67 + count_76 + count_77;
  printf("total color bytes = %d\n",total);
  printf("filler bytes = %d\n",bytes_scanned - total);

  free(buf);
  close(fhndl);

  return 0;
}
