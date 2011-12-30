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

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: undump infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int bytes_in_line(char *line,int line_len);
void convert_line(char *line,int line_len,char *buf);

int main(int argc,char **argv)
{
  FILE *fptr;
  int fhndl;
  int line_len;
  int line_no;
  int tot_bytes;
  char *buf;
  int buf_ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],argv[2])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 3;
  }

  if ((fhndl = open(argv[2],
    O_CREAT | O_EXCL | O_BINARY | O_WRONLY,
    S_IREAD | S_IWRITE)) == -1) {
    printf(couldnt_open,argv[2]);
    fclose(fptr);
    return 4;
  }

  line_no = 0;
  tot_bytes = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    tot_bytes += bytes_in_line(line,line_len);
  }

  if ((buf = (char *)malloc(tot_bytes)) == NULL) {
    printf("malloc of %d bytes failed\n",tot_bytes);
    close(fhndl);
    fclose(fptr);
    return 5;
  }

  fseek(fptr,0L,SEEK_SET);

  line_no = 0;
  buf_ix = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    convert_line(line,line_len,&buf[buf_ix]);

    buf_ix += 16;
  }

  if (write(fhndl,buf,tot_bytes) != tot_bytes) {
    printf("write failed\n");
    free(buf);
    close(fhndl);
    fclose(fptr);
    return 6;
  }

  free(buf);
  close(fhndl);
  fclose(fptr);

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

int bytes_in_line(char *line,int line_len)
{
  int m;
  int n;

  n = 0;
  m = 10;

  for ( ; ; ) {
    if (line[m+1] == ' ')
      break;

    n++;

    if (n == 16)
      break;

    m += 3;
  }

  return n;
}

void convert_line(char *line,int line_len,char *buf)
{
  int m;
  int n;
  int chara;

  n = 0;
  m = 10;

  for ( ; ; ) {
    if (line[m+1] == ' ')
      break;

    line[m+3] = 0;
    sscanf(&line[m+1],"%x",&chara);

    buf[n] = (char)chara;

    n++;

    if (n == 16)
      break;

    m += 3;
  }
}
