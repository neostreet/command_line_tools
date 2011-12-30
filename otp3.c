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

#define MAX_FILE_NAME_LEN 1024

#define BYTES_PER_LINE 16

static char usage[] = "usage: otp3 (-debug) padfile infile (infile ...)\n";

static char tempname[4096];

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";

int do_otp(char *buf,char *padbuf,int padfile_bytes,int out_fhndl);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  int bDebug;
  struct stat statbuf;
  off_t padfile_bytes;
  off_t infile_bytes;
  off_t bytes_to_read;
  char *padbuf;
  char *buf;
  int pad_fhndl;
  int in_fhndl;
  int out_fhndl;
  int bytes_read;
  int num_reads;
  int retval;

  if (argc < 3) {
    printf(usage);
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 2;
  }

  sprintf(tempname,"%s\\otp3.tmp",getenv("TEMP"));

  if (bDebug)
    printf("tempname = %s\n",tempname);

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 3;
  }

  padfile_bytes = statbuf.st_size;

  if ((padbuf = (char *)malloc(padfile_bytes)) == NULL) {
    printf(malloc_failed,padfile_bytes);
    return 4;
  }

  if ((buf = (char *)malloc(padfile_bytes)) == NULL) {
    printf(malloc_failed,padfile_bytes);
    return 5;
  }

  if ((pad_fhndl = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    free(padbuf);
    return 6;
  }

  bytes_read = read(pad_fhndl,padbuf,padfile_bytes);

  if (bytes_read != padfile_bytes) {
    printf(read_failed,argv[curr_arg],padfile_bytes);
    free(padbuf);
    return 7;
  }

  close(pad_fhndl);

  for (n = curr_arg + 1; n < argc; n++) {
    if (!strcmp(argv[curr_arg],argv[n])) {
      printf("infile and padfile must differ\n");
      continue;
    }

    if (stat(argv[n],&statbuf) == -1) {
      printf(couldnt_get_status,argv[n]);
      continue;
    }

    infile_bytes = statbuf.st_size;

    if ((in_fhndl = open(argv[n],O_BINARY | O_RDONLY,0)) == -1) {
      printf(couldnt_open,argv[n]);
      continue;
    }

    if (bDebug)
      printf("%s\n",argv[n]);

    if ((out_fhndl = open(tempname,
        O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
        S_IREAD | S_IWRITE)) == -1) {
      printf(couldnt_open,tempname);
      free(buf);
      free(padbuf);
      return 8;
    }

    num_reads = (infile_bytes + (padfile_bytes - 1)) / padfile_bytes;

    for (m = 0; m < num_reads; m++) {
      if (m < num_reads - 1)
        bytes_to_read = padfile_bytes;
      else
        bytes_to_read = infile_bytes - (m * padfile_bytes);

      bytes_read = read(in_fhndl,buf,bytes_to_read);

      if (bytes_read != bytes_to_read) {
        printf(read_failed,argv[n],bytes_to_read);
        free(buf);
        free(padbuf);
        return 9;
      }

      retval = do_otp(buf,padbuf,bytes_to_read,out_fhndl);

      if (retval) {
        printf("write of %d bytes failed\n",bytes_to_read);
        free(buf);
        free(padbuf);
        return 10;
      }
    }

    close(in_fhndl);
    close(out_fhndl);

    sprintf(buf,"touch -r %s %s",argv[n],tempname);
    system(buf);

    remove(argv[n]);
    rename(tempname,argv[n]);
  }

  free(buf);
  free(padbuf);

  return 0;
}

int do_otp(char *buf,char *padbuf,int padfile_bytes,int out_fhndl)
{
  int n;
  int bytes_written;

  for (n = 0; n < padfile_bytes; n++) {
    buf[n] = buf[n] ^ padbuf[n];
  }

  bytes_written = write(out_fhndl,buf,padfile_bytes);

  if (bytes_written != padfile_bytes)
    return 1;

  return 0;
}
