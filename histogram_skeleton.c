#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_RANGE 100
static int counts[MAX_RANGE];

static char usage[] =
"usage: histogram_skeleton (-verbose) (-sort) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bSort;
  FILE *fptr;
  int line_len;
  int val;
  int ixs[MAX_RANGE];
  int total;
  double dwork;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-sort"))
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

  for (n = 0; n < MAX_RANGE; n++)
    counts[n] = 0;

  if (bVerbose)
    total = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&val);

    if ((val >= 0) && (val < MAX_RANGE))
      counts[val]++;

    if (bVerbose)
      total++;
  }

  fclose(fptr);

  for (n = 0; n < MAX_RANGE; n++)
    ixs[n] = n;

  if (bSort)
    qsort(ixs,MAX_RANGE,sizeof (int),elem_compare);

  for (n = 0; n < MAX_RANGE; n++) {
    if (counts[ixs[n]]) {
      if (!bVerbose)
        printf("%d %d\n",ixs[n],counts[ixs[n]]);
      else {
        dwork = (double)counts[ixs[n]] / (double)total;
        printf("%d %d (%6.2lf)\n",ixs[n],counts[ixs[n]],dwork);
      }
    }
  }

  if (bVerbose)
    printf("\ntotal: %d\n",total);

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

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return counts[ix2] - counts[ix1];
}
