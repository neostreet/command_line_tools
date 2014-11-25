#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
#define MAX_CONTIG 10
static char line[MAX_CONTIG][MAX_LINE_LEN];

static char usage[] =
"usage: twin_lines gap filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int gap;
  int contig;
  FILE *fptr;
  int line_len;
  int line_no;
  int curr_ix;
  int compare_ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&gap);

  contig = gap + 1;

  if (contig > MAX_CONTIG) {
    printf("gap must be <= %d\n",MAX_CONTIG - 1);
    return 2;
  }

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    curr_ix = line_no % contig;

    GetLine(fptr,line[curr_ix],&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no >= contig) {
      compare_ix = curr_ix - gap;

      if (compare_ix < 0)
        compare_ix += contig;

      if (!strcmp(line[curr_ix],line[compare_ix]))
        printf("%7d %s\n",line_no,line[curr_ix]);
    }
  }

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
