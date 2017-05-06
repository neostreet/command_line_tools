#include <stdio.h>

#define NUM_FILES 2

#define MAX_LINE_LEN 4096
static char line[NUM_FILES][MAX_LINE_LEN];

static char usage[] = "usage: diff_files file1 file2\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr[NUM_FILES];
  int line_len[NUM_FILES];
  int line_no[NUM_FILES];
  int val[NUM_FILES];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_FILES; n++) {
    if ((fptr[n] = fopen(argv[1+n],"r")) == NULL) {
      printf(couldnt_open,argv[1+n]);
      return 2;
    }

    line_no[n] = 0;
  }

  for ( ; ; ) {
    for (n = 0; n < NUM_FILES; n++) {
      GetLine(fptr[n],line[n],&line_len[n],MAX_LINE_LEN);

      if (feof(fptr[n]))
        break;

      line_no[n]++;
      sscanf(line[n],"%d",&val[n]);
    }

    if (n < NUM_FILES)
      break;

    printf("%d\n",val[1] - val[0]);
  }

  for (n = 0; n < NUM_FILES; n++)
    fclose(fptr[n]);

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
