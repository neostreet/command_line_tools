#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

#define MAX_DOUBLES 500
static double doubles[MAX_DOUBLES];

static char usage[] = "usage: std_dev (-verbose) (-population) filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bPopulation;
  int num_files;
  int num_doubles;
  FILE *fptr;
  int linelen;
  double tot;
  double avg;
  double numerator;
  double work;
  double std_dev;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bPopulation = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-population"))
      bPopulation = true;
    else
      break;
  }

  num_files = argc - curr_arg;

  if (num_files < 1) {
    printf(usage);
    return 2;
  }

  for ( ; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      continue;
    }

    if (bVerbose)
      printf("%s\n",argv[curr_arg]);

    num_doubles = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (num_doubles == MAX_DOUBLES) {
        printf("MAX_DOUBLES value of %d exceeded\n",MAX_DOUBLES);
        return 3;
      }

      sscanf(line,"%lf",&doubles[num_doubles++]);
    }

    fclose(fptr);

    tot = 0;

    for (n = 0; n < num_doubles; n++)
      tot += doubles[n];

    avg = tot / (double)num_doubles;

    numerator = (double)0;

    for (n = 0; n < num_doubles; n++) {
      work = doubles[n] - avg;
      work *= work;
      numerator += work;
    }

    if (!bPopulation)
      numerator /= (double)(num_doubles - 1);
    else
      numerator /= (double)num_doubles;

    std_dev = sqrt(numerator);

    if (num_files == 1)
      printf("%lf\n",std_dev);
    else
      printf("%s: %lf\n",argv[curr_arg],std_dev);
  }

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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
