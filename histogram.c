#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: histogram (-sort) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char malloc_fail[] = "malloc of %d ints failed\n";

#define MAX_HISTOGRAM_VALUES 21

struct histogram {
  int value;
  int observations;
};

static struct histogram word_len_histogram[MAX_HISTOGRAM_VALUES];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bSort;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int num_histogram_values;
  int total_words;
  int total_letters;
  int *ixs;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
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

  if ((ixs = (int *)malloc(num_histogram_values * sizeof (int))) == NULL) {
    printf(malloc_fail,num_histogram_values);
    return 4;
  }

  for (n = 0; n < num_histogram_values; n++)
    ixs[n] = n;

  if (bSort)
    qsort(ixs,num_histogram_values,sizeof (int),compare);

  total_words = 0;

  for (n = 0; n < num_histogram_values; n++) {
    printf("%3d %3d %5d\n",
      word_len_histogram[ixs[n]].value,
      word_len_histogram[ixs[n]].observations,
      word_len_histogram[ixs[n]].value * word_len_histogram[ixs[n]].observations);
    total_words += word_len_histogram[ixs[n]].observations;
  }

  printf("\n    %3d %5d\n",total_words,total_letters);

  free(ixs);

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

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  return word_len_histogram[int1].value -
    word_len_histogram[int2].value;
}
