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

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: addfiles filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int read_ints_file(char *filename,int **ints_ptr,int *num_ints_ptr);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int filename_len;
  int num_files;
  int *ints;
  int num_ints;
  int save_num_ints;
  int retval;
  int malloc_size;
  int *totals;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  num_files = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    num_files++;

    retval = read_ints_file(filename,&ints,&num_ints);

    if (retval) {
      printf("read_ints_file() failed: %d\n",retval);
      return 3;
    }

    if (num_files == 1) {
      malloc_size = num_ints * sizeof (int);

      if ((totals = (int *)malloc(malloc_size)) == NULL) {
        printf("malloc of %d bytes failed\n",malloc_size);
        return 4;
      }

      for (n = 0; n < num_ints; n++)
        totals[n] = 0;

      save_num_ints = num_ints;
    }

    if (num_ints != save_num_ints) {
      free(ints);
      free(totals);
      printf("%s has the wrong size\n",filename);
      return 5;
    }

    for (n = 0; n < num_ints; n++)
      totals[n] += ints[n];

    free(ints);
  }

  fclose(fptr);

  for (n = 0; n < num_ints; n++)
    printf("%d\n",totals[n]);

  free(totals);

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}

int read_ints_file(char *filename,int **ints_ptr,int *num_ints_ptr)
{
  int m;
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  int *ints;
  int bytes_read;

  if (stat(filename,&statbuf) == -1) {
    printf(couldnt_get_status,filename);
    return 1;
  }

  bytes_to_io = (int)statbuf.st_size;

  if (bytes_to_io % sizeof(int)) {
    printf("the length of %s is not divisible by sizeof(int)\n",filename);
    return 2;
  }

  *num_ints_ptr = bytes_to_io / sizeof(int);

  if ((ints = (int *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 3;
  }

  if ((fhndl = open(filename,O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,filename);
    free(ints);
    return 4;
  }

  bytes_read = read(fhndl,ints,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(ints);
    close(fhndl);
    return 4;
  }

  close(fhndl);

  *ints_ptr = ints;

  return 0;
}
