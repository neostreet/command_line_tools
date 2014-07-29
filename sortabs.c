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
"usage: sortabs (-descending) (-line_numbers) (-streak) filename\n";

static bool bDescending;

static int *vals;

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bLineNumbers;
  bool bStreak;
  int ix;
  FILE *fptr;
  int num_vals;
  int work;
  int *ixs;
  int streak_count;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDescending = false;
  bLineNumbers = false;
  bStreak = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = true;
    else if (!strcmp(argv[curr_arg],"-line_numbers"))
      bLineNumbers = true;
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

  num_vals = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    num_vals++;
  }

  if ((vals = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d structs\n",num_vals);
    return 4;
  }

  if ((ixs = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_vals);
    free(vals);
    return 5;
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

  if (!bStreak) {
    for (n = 0; n < num_vals; n++) {
      if (!bLineNumbers)
        printf("%d\n",vals[ixs[n]]);
      else
        printf("%d (%d)\n",vals[ixs[n]],ixs[n]+1);
    }
  }
  else {
    streak_count = 0;

    for (n = 0; n < num_vals; n++) {
      if (!bDescending) {
        if (vals[ixs[n]] < 0)
          streak_count++;
        else
          break;
      }
      else {
        if (vals[ixs[n]] > 0)
          streak_count++;
        else
          break;
      }
    }

    printf("%d %s/%s\n",streak_count,save_dir,argv[curr_arg]);
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

  if (!bDescending)
    return int1 - int2;

  return int2 - int1;
}
