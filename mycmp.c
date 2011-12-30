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

#define FALSE 0
#define TRUE  1

static char usage[] = "usage: mycmp (-verbose) file1 file2\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";
static char file_differs[] = "%s differs\n";
static char file_matches[] = "%s matches\n";

static int comp_files(char *file1,char *file2);

int main(int argc,char **argv)
{
  int curr_arg;
  int bVerbose;
  int retval;

  if ((argc != 3) && (argc != 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  retval = comp_files(argv[curr_arg],argv[curr_arg+1]);

  if ((retval == 3) || (retval == 7))
    printf(file_differs,argv[curr_arg+1]);
  else if (bVerbose)
    printf(file_matches,argv[curr_arg+1]);

  return 0;
}

static int comp_files(char *file1,char *file2)
{
  int m;
  int n;
  char *files[2];
  struct stat statbuf[2];
  off_t bytes_to_read;
  char *buf[2];
  int fhndl;
  int bytes_read;

  files[0] = file1;
  files[1] = file2;

  for (n = 0; n < 2; n++) {
    if (stat(files[n],&statbuf[n]) == -1) {
      printf(couldnt_get_status,files[n]);
      return 1;
    }
  }

  if (statbuf[0].st_mode & S_IFDIR)
    return 2;

  if (statbuf[0].st_size != statbuf[1].st_size)
    return 3;

  bytes_to_read = statbuf[0].st_size;

  for (n = 0; n < 2; n++) {
    if ((buf[n] = (char *)malloc(bytes_to_read)) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_read);

      for (m = 0; m < n; m++)
        free(buf[m]);

      return 4;
    }

    if ((fhndl = open(files[n],O_BINARY | O_RDONLY,0)) == -1) {
      printf(couldnt_open,files[n]);

      for (m = 0; m <= n; m++)
        free(buf[m]);

      return 5;
    }

    bytes_read = read(fhndl,buf[n],bytes_to_read);

    if (bytes_read != bytes_to_read) {
      printf("read of %d bytes failed\n",bytes_to_read);

      for (m = 0; m <= n; m++)
        free(buf[m]);

      return 6;
    }

    close(fhndl);
  }

  for (m = 0; m < bytes_to_read; m++) {
    if (buf[0][m] != buf[1][m])
      break;
  }

  for (n = 0; n < 2; n++)
    free(buf[n]);

  if (m < bytes_to_read)
    return 7;

  return 0;
}
