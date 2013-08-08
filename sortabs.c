#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

static char usage[] = "usage: sortabs (-descending) filename\n";

static int bDescending;
static int *ints;

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int ix;
  FILE *fptr;
  int num_ints;
  int work;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDescending = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 3;
  }

  num_ints = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    num_ints++;
  }

  if ((ints = (int *)malloc(num_ints * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_ints);
    return 4;
  }

  fseek(fptr,0L,SEEK_SET);

  ix = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    ints[ix++] = work;
  }

  fclose(fptr);

  qsort(ints,num_ints,sizeof (int),compare);

  for (n = 0; n < num_ints; n++)
    printf("%d\n",ints[n]);

  free(ints);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  if (int1 < 0)
    int1 *= -1;

  if (int2 < 0)
    int2 *= -1;

  if (!bDescending)
    return int1 - int2;

  return int2 - int1;
}
