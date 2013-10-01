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
#endif

static char usage[] = "usage: sortdbls (-descending) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static bool bDescending;
static double *dbls;

static int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int ix;
  int num_dbls;
  int work;
  struct stat statbuf;
  int bytes_to_io;
  int bytes_read;
  char *buf;
  int fhndl;
  int dbl_ix;
  char **cppt;
  int *ixs;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDescending = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 3;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 4;
  }

  if ((fhndl = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    free(buf);
    return 5;
  }

  bytes_read = read(fhndl,buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl);
    return 6;
  }

  close(fhndl);

  num_dbls = 0;

  for (n = 0; n < bytes_read; n++) {
    if (buf[n] == '\n')
      num_dbls++;
  }

  if ((dbls = (double *)malloc(num_dbls * sizeof (double))) == NULL) {
    printf("couldn't malloc %d dbls\n",num_dbls);
    free(buf);
    return 7;
  }

  if ((cppt = (char **)malloc(num_dbls * sizeof (char *))) == NULL) {
    printf("couldn't malloc %d pointers\n",num_dbls);
    free(dbls);
    free(buf);
    return 8;
  }

  if ((ixs = (int *)malloc(num_dbls * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_dbls);
    free(cppt);
    free(dbls);
    free(buf);
    return 9;
  }

  m = 0;
  dbl_ix = 0;

  for (n = 0; n < bytes_read; n++) {
    if (buf[n] == '\n') {
      buf[n] = 0;
      cppt[dbl_ix] = &buf[m];
      sscanf(&buf[m],"%lf",&dbls[dbl_ix++]);
      m = n + 1;
    }
  }

  for (n = 0; n < num_dbls; n++)
    ixs[n] = n;

  qsort(ixs,num_dbls,sizeof (int),compare);

  for (n = 0; n < num_dbls; n++)
    printf("%s\n",cppt[ixs[n]]);

  free(ixs);
  free(cppt);
  free(dbls);
  free(buf);

  return 0;
}

static int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  if (!bDescending) {
    if (dbls[ix1] < dbls[ix2])
      return -1;
    else if (dbls[ix1] > dbls[ix2])
      return 1;

    return 0;
  }
  else {
    if (dbls[ix2] < dbls[ix1])
      return -1;
    else if (dbls[ix2] > dbls[ix1])
      return 1;

    return 0;
  }
}
