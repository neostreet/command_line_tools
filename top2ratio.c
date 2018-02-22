#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

static char usage[] = "usage: top2ratio filename\n";

static int *vals;

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int ix;
  FILE *fptr;
  int retval;
  int num_vals;
  int work;
  int *ixs;
  int top_vals[2];
  int top_ix;
  double dwork;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return 7;
  }

  num_vals = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    num_vals++;
  }

  if ((vals = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d structs\n",num_vals);
    return 8;
  }

  if ((ixs = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_vals);
    free(vals);
    return 9;
  }

  fseek(fptr,0L,SEEK_SET);

  ix = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    vals[ix++] = work;
  }

  fclose(fptr);

  for (n = 0; n < num_vals; n++)
    ixs[n] = n;

  qsort(ixs,num_vals,sizeof (int),compare);

  top_ix = 0;

  for (n = 0; n < num_vals; n++) {
    if (vals[ixs[n]] > 0) {
      top_vals[top_ix++] = vals[ixs[n]];

      if (top_ix == 2)
        break;
    }
  }

  if (top_ix == 2) {
    dwork = (double)top_vals[0] / (double)top_vals[1];

    printf("%lf (%d %d) %s\n",dwork,top_vals[0],top_vals[1],save_dir);
  }

  free(ixs);
  free(vals);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;
  int int1;
  int int2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  int1 = vals[ix1];
  int2 = vals[ix2];

  if (int1 < 0)
    int1 *= -1;

  if (int2 < 0)
    int2 *= -1;

  return int2 - int1;
}
