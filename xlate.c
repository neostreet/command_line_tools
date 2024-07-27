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

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

struct xlate {
  char from;
  char to;
};

#define MAX_XLATES 10
static struct xlate xlates[MAX_XLATES];

static char usage[] = "usage: xlate xlate_file infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int num_xlates;
  struct stat statbuf;
  int bytes_to_io;
  int bytes_read;
  char *buf;
  int fhndl[2];
  int remainder;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[2],argv[3])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 3;
  }

  num_xlates = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%x %x",&xlates[num_xlates].from,&xlates[num_xlates].to);

    num_xlates++;

    if (num_xlates == MAX_XLATES)
      break;
  }

  fclose(fptr);

  if (stat(argv[2],&statbuf) == -1) {
    printf(couldnt_get_status,argv[2]);
    return 4;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 5;
  }

  if ((fhndl[0] = open(argv[2],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[2]);
    free(buf);
    return 6;
  }

  if ((fhndl[1] = open(argv[3],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[3]);
    free(buf);
    close(fhndl[0]);
    return 7;
  }

  bytes_read = read(fhndl[0],buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 8;
  }

  for (n = 0; n < bytes_to_io; n++) {
    remainder = n % 652;

    if ((remainder != 650) && (remainder != 651)) {
      for (m = 0; m < num_xlates; m++) {
        if (buf[n] == xlates[m].from) {
          buf[n] = xlates[m].to;
          break;
        }
      }
    }
  }

  if (write(fhndl[1],buf,bytes_to_io) != bytes_to_io) {
    printf("write failed\n");
    free(buf);
    close(fhndl[0]);
    close(fhndl[1]);
    return 9;
  }

  free(buf);
  close(fhndl[0]);
  close(fhndl[1]);

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
