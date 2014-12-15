#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

#define MAX_SUBSET_SIZE 100
static int subset_values[MAX_SUBSET_SIZE];

static char usage[] =
"usage: moving_sum (-verbose) subset_size filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char fmt[] = "%10d\n";
static char fmt2[] = "%10d %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  int subset_size;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int work;
  int sum;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&subset_size);

  if (subset_size > MAX_SUBSET_SIZE) {
    printf("subset_size must be <= %d\n",MAX_SUBSET_SIZE);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 4;
  }

  line_no = 0;
  ix = 0;
  sum = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (line_no >= subset_size)
      sum -= subset_values[ix];

    sscanf(line,"%d",&subset_values[ix]);

    sum += subset_values[ix];

    line_no++;
    ix++;

    if (ix == subset_size)
      ix = 0;

    if (line_no >= subset_size) {
      if (!bVerbose)
        printf(fmt,sum);
      else
        printf(fmt2,sum,line);
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
