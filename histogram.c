#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: histogram filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

#define MAX_HISTOGRAM_VALUES 21

struct histogram {
  int value;
  int observations;
};

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  struct histogram word_len_histogram[MAX_HISTOGRAM_VALUES];
  int num_histogram_values;
  int total_letters;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;
  num_histogram_values = 0;
  total_letters = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    total_letters += line_len;

    for (n = 0; n < num_histogram_values; n++) {
      if (word_len_histogram[n].value == line_len)
        break;
    }

    if (n == num_histogram_values) {
      word_len_histogram[n].value = line_len;
      word_len_histogram[n].observations = 1;
      num_histogram_values++;
    }
    else
      word_len_histogram[n].observations++;
  }

  fclose(fptr);

  for (n = 0; n < num_histogram_values; n++) {
    printf("%3d %3d %5d\n",
      word_len_histogram[n].value,
      word_len_histogram[n].observations,
      word_len_histogram[n].value * word_len_histogram[n].observations);
  }

  printf("        %5d\n",total_letters);

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
