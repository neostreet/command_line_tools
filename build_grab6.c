#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define WIDTH     0
#define HEIGHT    1
#define COLOR     2
#define NUM_PARMS 3

static char usage[] = "usage: build_grab6 width height color filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char fmt[] = "%08x\n";
static char white[] = "00ffffff";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int parms[NUM_PARMS];
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  printf("0\n");
  printf("0\n");

  for (n = 0; n < NUM_PARMS; n++) {
    if (n < NUM_PARMS - 1) {
      sscanf(argv[1 + n],"%d",&parms[n]);
      printf("%d\n",parms[n]);
    }
    else
      sscanf(argv[1 + n],"%x",&parms[n]);
  }

  if ((fptr = fopen(argv[NUM_PARMS + 1],"r")) == NULL) {
    printf(couldnt_open,argv[NUM_PARMS + 1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_len != (parms[WIDTH] * 2) - 1) {
      printf("line %d has the wrong width\n",line_no);
      return 3;
    }

    for (n = 0; n < line_len; n+= 2) {
      if (line[n] != '.')
        printf(fmt,parms[COLOR]);
      else
        printf("%s\n",white);
    }
  }

  fclose(fptr);

  if (line_no != parms[HEIGHT]) {
    printf("wrong number of lines in %s\n",argv[NUM_PARMS + 1]);
    return 4;
  }

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
