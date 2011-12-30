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

static char usage[] = "usage: split filename bytes_per_splitfile\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char *buf;
static char *splitname;

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  off_t bytes_to_do;
  off_t bytes_per_splitfile;
  int num_splits;
  int splitname_len;
  int fhndl1;
  int fhndl2;
  int bytes_read;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  bytes_to_do = statbuf.st_size;

  sscanf(argv[2],"%d",&bytes_per_splitfile);

  if ((buf = (char *)malloc(bytes_per_splitfile)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_per_splitfile);
    return 3;
  }

  num_splits = (bytes_to_do + bytes_per_splitfile - 1) / bytes_per_splitfile;

  splitname_len = strlen(argv[1]) +
    5;  /* account for ".999" and trailing NULL */

  if ((splitname = (char *)malloc(splitname_len)) == NULL) {
    printf("malloc of %d bytes failed\n",splitname_len);
    free(buf);
    return 4;
  }

  if ((fhndl1 = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    free(splitname);
    return 5;
  }

  for (n = 0; n < num_splits; n++) {
    sprintf(splitname,"%s.%03d",argv[1],n);

    if ((fhndl2 = open(splitname,
      O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
      S_IREAD | S_IWRITE)) == -1) {

      printf(couldnt_open,splitname);
      free(buf);
      free(splitname);
      return 6;
    }

    bytes_read = read(fhndl1,buf,bytes_per_splitfile);
    write(fhndl2,buf,bytes_read);

    close(fhndl2);
  }

  close(fhndl1);

  free(buf);
  free(splitname);

  return 0;
}
