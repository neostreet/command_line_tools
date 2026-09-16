#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: remove_spaces (-debug) infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int remove_spaces(char *line,int line_len,bool bDebug);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  FILE *fptr;
  FILE *out_fptr;
  int line_len;
  int line_no;
  int count;
  int total_count;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
       break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg],argv[curr_arg+1])) {
    printf("outfile must differ from infile\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  if ((out_fptr = fopen(argv[curr_arg+1],"w")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 5;
  }

  total_count = 0;
  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    count = remove_spaces(line,line_len,bDebug);
    total_count += count;
    fprintf(out_fptr,"%s\n",line);
  }

  fclose(fptr);
  fclose(out_fptr);

  printf("%d\n",total_count);

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

static int remove_spaces(char *line,int line_len,bool bDebug)
{
  int m;
  int n;

  for (m = 0,n = 0; n < line_len; n++) {
    if (line[n] != ' ') {
      if (m != n) {
        if (bDebug)
          printf("character copied from offset %d to offset %d\n",n,m);

        line[m++] = line[n];
      }
      else {
        if (bDebug)
          printf("no need to copy to offset %d\n",m);

        m++;
      }
    }
    else if (bDebug)
      printf("space found at offset %d, m = %d\n",n,m);
  }

  line[m] = 0;

  if (bDebug) {
    printf("original line length: %d\n",line_len);
    printf("new line length: %d\n",m);
    printf("spaces removed: %d\n",line_len - m);
  }

  return line_len - m;
}
