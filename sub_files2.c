#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[2][MAX_LINE_LEN];

#define NUM_FILS 2
#define NUM_VALS 3

#define DATE_LEN 10

static char usage[] = "usage: sub_files2 filename filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr[NUM_FILS];
  int line_len[NUM_FILS];
  int line_no;
  bool bBreak;
  char dates[NUM_FILS][DATE_LEN+1];
  int file_vals[NUM_FILS][NUM_VALS];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  for (n = 0; n < NUM_FILS; n++) {
    if ((fptr[n] = fopen(argv[1 + n],"r")) == NULL) {
      printf(couldnt_open,argv[1 +n]);
      return 2;
    }
  }

  line_no = 0;
  bBreak = false;

  for ( ; ; ) {
    for (n = 0; n < NUM_FILS; n++) {
      GetLine(fptr[n],line[n],&line_len[n],MAX_LINE_LEN);

      if (feof(fptr[n])) {
        bBreak = true;
        break;
      }

      sscanf(&line[n][0],"%s %d %d %d",
        &dates[n][0],
        &file_vals[n][0],
        &file_vals[n][1],
        &file_vals[n][2]);
    }

    if (bBreak)
      break;

    line_no++;

    printf("%s %10d %10d %10d\n",
      dates[0],
      file_vals[0][0] - file_vals[1][0],
      file_vals[0][1] - file_vals[1][1],
      file_vals[0][2] - file_vals[1][2]);
  }

  for (n = 0; n < NUM_FILS; n++)
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
