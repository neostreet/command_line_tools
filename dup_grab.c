#include <windows.h>
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

#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4

static char usage[] = "usage: dup_grab filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char fmt[] = "%08x\n";

static COLORREF *image;

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int parms[NUM_PARMS];
  int nobs;
  int bytes_to_malloc;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&parms[line_no]);

    line_no++;

    if (line_no == NUM_PARMS)
      break;
  }

  nobs = parms[WIDTH] * parms[HEIGHT];

  bytes_to_malloc = nobs * sizeof (COLORREF);

  if ((image = (COLORREF *)malloc(bytes_to_malloc)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_malloc);
    return 3;
  }

  for (n = 0; n < nobs; n++) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%x",&image[n]);
  }

  for (n = 0; n < NUM_PARMS; n++)
    printf("%d\n",parms[n]);

  for (n = 0; n < nobs; n++)
    printf(fmt,image[n]);

  free(image);

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
