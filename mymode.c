#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKETS 500
static int buckets[MAX_BUCKETS];
static int ixs[MAX_BUCKETS];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: mymode (-verbose) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int max;
  int num_elems;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for (n = 0; n < MAX_BUCKETS; n++)
    buckets[n] = 0;

  line_no = 0;
  max = -1;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&val);

    if ((val < 0) || (val >= MAX_BUCKETS)) {
      printf("invalid value on line %d: %d\n",line_no,val);
      return 4;
    }

    if (val > max)
      max = val;

    buckets[val]++;
  }

  fclose(fptr);

  num_elems = max + 1;

  for (n = 0; n < num_elems; n++)
    ixs[n] = n;

  qsort(ixs,num_elems,sizeof (int),compare);

  if (!bVerbose)
    printf("mode = %d (%d)\n",ixs[0],buckets[ixs[0]]);
  else {
    for (n = 0; n < num_elems; n++) {
      if (!buckets[ixs[n]])
        break;
      else
        printf("%d (%d)\n",ixs[n],buckets[ixs[n]]);
    }
  }

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

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return buckets[ix2] - buckets[ix1];
}
