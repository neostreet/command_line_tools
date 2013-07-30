#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: histogram (-sort) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char malloc_fail[] = "malloc of %d ints failed\n";

struct histogram {
  int value;
  int observations;
};

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bSort;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  list<struct histogram> word_len_histogram;
  list<struct histogram>::iterator it;
  bool bUpserted;
  struct histogram work;
  int total_words;
  int total_letters;

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

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;
    bUpserted = false;

    for (it = word_len_histogram.begin(); it != word_len_histogram.end(); it++) {
      if (line_len == it->value) {
        it->observations++;
        bUpserted = true;
        break;
      }
      else if (line_len < it->value) {
        work.value = line_len;
        work.observations = 1;
        word_len_histogram.insert(it,work);
        bUpserted = true;
        break;
      }
    }

    if (!bUpserted) {
      work.value = line_len;
      work.observations = 1;
      word_len_histogram.push_back(work);
    }
  }

  fclose(fptr);

  total_words = 0;
  total_letters = 0;

  for (it = word_len_histogram.begin(); it != word_len_histogram.end(); it++) {
    printf("%3d %3d %5d\n",
      it->value,
      it->observations,
      it->value * it->observations);
    total_words += it->observations;
    total_letters += it->value * it->observations;
  }

  printf("\n    %3d %5d\n",total_words,total_letters);

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
