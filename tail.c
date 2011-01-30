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

static char usage[] = "usage: tail filename bytes_in_tail\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char *buf;
#define MAX_TAILNAME_LEN 512
static char tailname[MAX_TAILNAME_LEN+1];

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  off_t bytes_in_tail;
  off_t bytes_to_seek_past;
  int retval;
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

  sscanf(argv[2],"%d",&bytes_in_tail);

  if (statbuf.st_size < bytes_in_tail) {
    printf("bytes_in_tail must be <= file size\n");
    return 3;
  }

  bytes_to_seek_past = statbuf.st_size - bytes_in_tail;

  if ((buf = (char *)malloc(bytes_in_tail)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_in_tail);
    return 4;
  }

  if ((fhndl1 = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    free(buf);
    return 5;
  }

  sprintf(tailname,"%s.tail",argv[1]);

  if ((fhndl2 = open(tailname,
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {

    printf(couldnt_open,tailname);
    free(buf);
    return 6;
  }

  if (bytes_to_seek_past) {
    retval = lseek(fhndl1,bytes_to_seek_past,SEEK_SET);

    if (retval == -1) {
      printf("lseek() failed\n");
      return 7;
    }
  }

  bytes_read = read(fhndl1,buf,bytes_in_tail);
  write(fhndl2,buf,bytes_read);

  close(fhndl2);

  close(fhndl1);

  free(buf);

  return 0;
}
