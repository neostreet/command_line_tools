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

static char usage[] =
"usage: sortabs (-descending) (-verbose) (-streak) filename\n";

static bool bDescending;

struct val_ix {
  int val;
  int ix;
};

static struct val_ix *val_ixs;

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bStreak;
  int ix;
  FILE *fptr;
  int num_val_ixs;
  int work;
  int *ixs;
  int streak_count;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDescending = false;
  bVerbose = false;
  bStreak = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-streak"))
      bStreak = true;
    else
      break;
  }

  if (bStreak)
    getcwd(save_dir,_MAX_PATH);

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 3;
  }

  num_val_ixs = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    num_val_ixs++;
  }

  if ((val_ixs = (struct val_ix *)malloc(num_val_ixs * sizeof (struct val_ix))) == NULL) {
    printf("couldn't malloc %d structs\n",num_val_ixs);
    return 4;
  }

  if ((ixs = (int *)malloc(num_val_ixs * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_val_ixs);
    free(val_ixs);
    return 5;
  }

  fseek(fptr,0L,SEEK_SET);

  ix = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    val_ixs[ix].val = work;
    val_ixs[ix].ix = ix;
    ix++;
  }

  fclose(fptr);

  for (n = 0; n < num_val_ixs; n++)
    ixs[n] = n;

  qsort(ixs,num_val_ixs,sizeof (int),compare);

  if (!bStreak) {
    for (n = 0; n < num_val_ixs; n++) {
      if (!bVerbose)
        printf("%d\n",val_ixs[ixs[n]].val);
      else
        printf("%d (%d)\n",val_ixs[ixs[n]].val,val_ixs[ixs[n]].ix+1);
    }
  }
  else {
    streak_count = 0;

    for (n = 0; n < num_val_ixs; n++) {
      if (!bDescending) {
        if (val_ixs[ixs[n]].val < 0)
          streak_count++;
        else
          break;
      }
      else {
        if (val_ixs[ixs[n]].val > 0)
          streak_count++;
        else
          break;
      }
    }

    printf("%d %s/%s\n",streak_count,save_dir,argv[curr_arg]);
  }

  free(ixs);
  free(val_ixs);

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

  int1 = val_ixs[ix1].val;
  int2 = val_ixs[ix2].val;

  if (int1 < 0)
    int1 *= -1;

  if (int2 < 0)
    int2 *= -1;

  if (!bDescending)
    return int1 - int2;

  return int2 - int1;
}
