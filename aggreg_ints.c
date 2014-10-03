#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: aggreg_ints max_int filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_malloc[] ="couldn't malloc %d ints\n";

static int *int_counts;

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int max_int;
  int *ixs;
  FILE *fptr;
  int line_len;
  int line_no;
  int work;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&max_int);

  int_counts = (int *)malloc((max_int + 1) * sizeof (int));

  if (int_counts == NULL) {
    printf(couldnt_malloc,max_int + 1);
    return 2;
  }

  ixs = (int *)malloc((max_int + 1) * sizeof (int));

  if (ixs == NULL) {
    printf(couldnt_malloc,max_int + 1);
    free(int_counts);
    return 3;
  }

  for (n = 0; n < max_int + 1; n++) {
    int_counts[n] = 0;
    ixs[n] = n;
  }

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    free(ixs);
    free(int_counts);
    return 4;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&work);

    if (work > max_int) {
      printf("max_int value of %d exceeded on line %d\n",max_int,line_no);
      free(ixs);
      free(int_counts);
      return 5;
    }

    int_counts[work]++;
  }

  fclose(fptr);

  qsort(ixs,max_int + 1,sizeof (int),compare);

  for (n = 0; n < max_int + 1; n++) {
    printf("%3d %5d\n",ixs[n],int_counts[ixs[n]]);
  }

  free(ixs);
  free(int_counts);

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
  int ix1;
  int ix2;
  int int1;
  int int2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  int1 = int_counts[ix1];
  int2 = int_counts[ix2];

  return int2 - int1;
}
