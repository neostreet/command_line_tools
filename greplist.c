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
#include "str_misc.h"

#define FALSE 0
#define TRUE  1

#define CARRIAGE_RETURN 0x0d
#define LINE_FEED       0x0a

static char usage[] = "usage: greplist (-not) (-nl) filename filename\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int do_greplist(char **buf,off_t *bytes_read,int bNot,int bNoLines);

int main(int argc,char **argv)
{
  int n;
  int bNot;
  int bNoLines;
  int curr_arg;
  struct stat statbuf[2];
  off_t bytes_to_read[2];
  int fhndl[2];
  char *buf[2];
  off_t bytes_read[2];
  int retval;

  bNot = FALSE;
  bNoLines = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-not"))
      bNot = TRUE;
    else if (!strcmp(argv[curr_arg],"-nl"))
      bNoLines = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < 2; n++) {
    if (stat(argv[curr_arg+n],&statbuf[n]) == -1) {
      printf(couldnt_get_status,argv[curr_arg+n]);
      return 2;
    }

    bytes_to_read[n] = statbuf[n].st_size;

    if ((buf[n] = (char *)malloc(bytes_to_read[n])) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_read[n]);
      return 3;
    }

    if ((fhndl[n] = open(argv[curr_arg+n],O_BINARY | O_RDONLY,0)) == -1) {
      printf(couldnt_open,argv[curr_arg+n]);
      free(buf[n]);
      return 4;
    }

    bytes_read[n] = read(fhndl[n],buf[n],bytes_to_read[n]);

    if (bytes_read[n] != bytes_to_read[n]) {
      printf("read of %d bytes failed\n",bytes_to_read[n]);
      return 5;
    }

    close(fhndl[n]);
  }

  retval = do_greplist(buf,bytes_read,bNot,bNoLines);

  if (retval)
    printf("do_greplist() failed: %d\n",retval);

  for (n = 0; n < 2; n++)
    free(buf[n]);

  return 0;
}

int do_greplist(char **buf,off_t *bytes_read,int bNot,int bNoLines)
{
  int m;
  int n;
  int num_lines[2];
  int *line_ixs[2];
  int work;
  int ix;

  for (n = 0; n < 2; n++) {
    work = 0;

    for (m = 0; m < bytes_read[n]; m++) {
#ifdef WIN32
      if (buf[n][m] == CARRIAGE_RETURN)
#else
      if (buf[n][m] == LINE_FEED)
#endif
        work++;
    }

    num_lines[n] = work;
    line_ixs[n] = (int *)malloc(work * sizeof (int));

    if (line_ixs[n] == NULL)
      return 1;

    work = 0;
    line_ixs[n][work] = 0;

    for (m = 0; m < bytes_read[n]; m++) {
#ifdef WIN32
      if (buf[n][m] == CARRIAGE_RETURN) {
#else
      if (buf[n][m] == LINE_FEED) {
#endif
        buf[n][m] = 0;
        work++;
#ifdef WIN32
        m++;
#endif

        if (work < num_lines[n])
          line_ixs[n][work] = m + 1;
      }
    }
  }

  for (n = 0; n < num_lines[1]; n++) {
    for (m = 0; m < num_lines[0]; m++) {
      ix = 0;

      if (find_substring(&buf[1][line_ixs[1][n]],&ix,
        &buf[0][line_ixs[0][m]],TRUE,FALSE)) {

        if (!bNot) {
          if (bNoLines)
            printf("%s\n",&buf[1][line_ixs[1][n]]);
          else
            printf("%03d:%s\n",n+1,&buf[1][line_ixs[1][n]]);
        }

        break;
      }
    }

    if (bNot) {
      if (m == num_lines[0]) {
        if (bNoLines)
          printf("%s\n",&buf[1][line_ixs[1][n]]);
        else
          printf("%03d:%s\n",n+1,&buf[1][line_ixs[1][n]]);
      }
    }
  }

  for (n = 0; n < 2; n++)
    free(line_ixs[n]);

  return 0;
}
