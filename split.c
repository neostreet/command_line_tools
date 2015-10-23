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
"usage: split (-max_splitsn) filename bytes_per_splitfile\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char *buf;
static char *splitname;

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int max_splits;
  struct stat statbuf;
  off_t bytes_to_do;
  int work;
  off_t bytes_per_splitfile;
  int num_splits;
  int splitname_len;
  int fhndl1;
  int fhndl2;
  int bytes_read;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  max_splits = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-max_splits",11)) {
      sscanf(&argv[curr_arg][11],"%d",&max_splits);
    }
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 3;
  }

  bytes_to_do = statbuf.st_size;

  sscanf(argv[curr_arg+1],"%d",&work);
  bytes_per_splitfile = work;

  if ((buf = (char *)malloc(bytes_per_splitfile)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_per_splitfile);
    return 4;
  }

  num_splits = (bytes_to_do + bytes_per_splitfile - 1) / bytes_per_splitfile;

  splitname_len = strlen(argv[curr_arg]) +
    5;  /* account for ".999" and trailing NULL */

  if ((splitname = (char *)malloc(splitname_len)) == NULL) {
    printf("malloc of %d bytes failed\n",splitname_len);
    free(buf);
    return 5;
  }

  if ((fhndl1 = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    free(buf);
    free(splitname);
    return 6;
  }

  for (n = 0; (n < num_splits) && ((max_splits == 0) || (n < max_splits)); n++) {
    sprintf(splitname,"%s.%03d",argv[curr_arg],n);

    if ((fhndl2 = open(splitname,
      O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
      S_IREAD | S_IWRITE)) == -1) {

      printf(couldnt_open,splitname);
      free(buf);
      free(splitname);
      return 7;
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
