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
"usage: sortabs (-descending) (-line_numbers) (-pos_streak) (-neg_streak)\n"
"  (-ends_with_a_bang) (-ends_with_a_big_bang) (-only_zero) (-only_nonzero)\n"
"  filename\n";

static bool bDescending;

static int *vals;

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bLineNumbers;
  bool bStreak;
  bool bPosStreak;
  bool bEndsWithABang;
  bool bEndsWithABigBang;
  bool bOnlyZero;
  bool bOnlyNonzero;
  int last_winning_hand_ix;
  bool bBang;
  int ix;
  FILE *fptr;
  int num_vals;
  int work;
  int *ixs;
  int streak_count;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bDescending = false;
  bLineNumbers = false;
  bStreak = false;
  bEndsWithABang = false;
  bEndsWithABigBang = false;
  bOnlyZero = false;
  bOnlyNonzero = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-descending"))
      bDescending = true;
    else if (!strcmp(argv[curr_arg],"-line_numbers"))
      bLineNumbers = true;
    else if (!strcmp(argv[curr_arg],"-pos_streak")) {
      bStreak = true;
      bPosStreak = true;
    }
    else if (!strcmp(argv[curr_arg],"-neg_streak")) {
      if (bStreak) {
        printf("can't specify both -pos_streak and -neg_streak\n");
        return 2;
      }

      bStreak = true;
      bPosStreak = false;
    }
    else if (!strcmp(argv[curr_arg],"-ends_with_a_bang")) {
      bEndsWithABang = true;
      bStreak = true;
      bPosStreak = true;
      bDescending = true;
    }
    else if (!strcmp(argv[curr_arg],"-ends_with_a_big_bang")) {
      bEndsWithABigBang = true;
      bStreak = true;
      bPosStreak = true;
      bDescending = true;
    }
    else if (!strcmp(argv[curr_arg],"-only_zero"))
      bOnlyZero = true;
    else if (!strcmp(argv[curr_arg],"-only_nonzero"))
      bOnlyNonzero = true;
    else
      break;
  }

  if (bEndsWithABang && bEndsWithABigBang) {
    printf("can't specify both -ends_with_a_bang and -ends_with_a_big_bang\n");
    return 3;
  }

  if (bOnlyZero && bOnlyNonzero) {
    printf("can't specify both -only_zero and -only_nonzero\n");
    return 4;
  }

  if (bStreak)
    getcwd(save_dir,_MAX_PATH);

  if (argc - curr_arg != 1) {
    printf(usage);
    return 5;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 6;
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
    return 7;
  }

  if ((ixs = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_vals);
    free(vals);
    return 8;
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

  if (bEndsWithABang || bEndsWithABigBang) {
    last_winning_hand_ix = -1;

    for (n = num_vals - 1; n >= 0; n--) {
      if (vals[n] > 0) {
        last_winning_hand_ix = n;
        break;
      }
    }
  }

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
    if (!bEndsWithABang && !bEndsWithABigBang) {
      streak_count = 0;

      for (n = 0; n < num_vals; n++) {
        if (!bPosStreak) {
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
    }
    else {
      bBang = false;

      for (n = 0; n < num_vals; n++) {
        if (vals[ixs[n]] > 0) {
          if (bEndsWithABang) {
            if (ixs[n] == last_winning_hand_ix)
              bBang = true;
          }
          else {
            if (!n && ixs[n] == last_winning_hand_ix)
              bBang = true;
          }
        }
        else
          break;
      }
    }

    if (!bEndsWithABang && !bEndsWithABigBang) {
      if (!bOnlyZero || !streak_count)
        if (!bOnlyNonzero || streak_count)
          printf("%d %s/%s\n",streak_count,save_dir,argv[curr_arg]);
    }
    else if (bBang)
      printf("%s/%s\n",save_dir,argv[curr_arg]);
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
