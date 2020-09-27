#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line0[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

#define MAX_STREAK 100
static int streak_histogram[MAX_STREAK];

static char usage[] =
"usage: streak (-debug) (-verbose) (-only_winning) (-only_losing)\n"
"  (-histogram) (-max_plus) (-max_minus) (-gevalue) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bOnlyWinning;
  bool bOnlyLosing;
  bool bHistogram;
  bool bMaxPlus;
  bool bMaxMinus;
  int ge_value;
  FILE *fptr;
  int line_len;
  int line_no;
  int plus_streak;
  int minus_streak;
  int max_plus_streak;
  int max_minus_streak;
  int work;
  int chara;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bOnlyWinning = false;
  bOnlyLosing = false;
  bHistogram = false;
  bMaxPlus = false;
  bMaxMinus = false;
  ge_value = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-only_winning"))
      bOnlyWinning = true;
    else if (!strcmp(argv[curr_arg],"-only_losing"))
      bOnlyLosing = true;
    else if (!strcmp(argv[curr_arg],"-histogram"))
      bHistogram = true;
    else if (!strcmp(argv[curr_arg],"-max_plus"))
      bMaxPlus = true;
    else if (!strcmp(argv[curr_arg],"-max_minus"))
      bMaxMinus = true;
    else if (!strncmp(argv[curr_arg],"-ge",3))
      sscanf(&argv[curr_arg][3],"%d",&ge_value);
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bOnlyWinning && bOnlyLosing) {
    printf("can't specify both -only_winning and -only_losing\n");
    return 3;
  }

  if (bMaxPlus && bMaxMinus) {
    printf("can't specify both -max and -min\n");
    return 4;
  }

  if (bMaxPlus || bMaxMinus)
    getcwd(save_dir,_MAX_PATH);

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  line_no = 0;
  plus_streak = 0;
  minus_streak = 0;
  max_plus_streak = 0;
  max_minus_streak = 0;

  if (bHistogram) {
    for (n = 0; n < MAX_STREAK; n++)
      streak_histogram[n] = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (!line_len)
      continue;

    chara = line[0];

    if (((chara >= 'a') && (chara <= 'z')) ||
        ((chara >= 'A') && (chara <= 'Z')))
      continue;

    line_no++;

    sscanf(line,"%d",&work);

    if (bDebug)
      printf("debug: %10d\n",work);

    if (line_no == 1) {
      if (work <= 0)
        minus_streak = 1;
      else
        plus_streak = 1;

      if (bVerbose) {
        strcpy(save_line0,line);
        strcpy(save_line,line);
      }

      continue;
    }

    if (work <= 0) {
      if (minus_streak)
        minus_streak++;
      else {
        if (bHistogram && (bOnlyWinning || !bOnlyLosing)) {
          if (plus_streak > MAX_STREAK) {
            printf("plus_streak (%d) > MAX_STREAK (%d) on lines %d\n",
              plus_streak,MAX_STREAK,line_no);
            return 6;
          }

          streak_histogram[plus_streak - 1]++;
        }

        if (!bOnlyLosing && !bMaxPlus && !bMaxMinus) {
          if (!bVerbose) {
            if (!bOnlyWinning) {
              if (plus_streak >= ge_value)
                printf("+%d\n",plus_streak);
            }
            else {
              if (plus_streak >= ge_value)
                printf("%d\n",plus_streak);
            }
          }
          else {
            if (!bOnlyWinning) {
              if (plus_streak >= ge_value)
                printf("+%d %s %s\n",plus_streak,save_line0,save_line);
            }
            else {
              if (plus_streak >= ge_value)
                printf("%d %s %s\n",plus_streak,save_line0,save_line);
            }
          }
        }

        if (plus_streak > max_plus_streak)
          max_plus_streak = plus_streak;

        plus_streak = 0;
        minus_streak = 1;
      }
    }
    else {
      if (plus_streak)
        plus_streak++;
      else {
        if (bHistogram && (bOnlyLosing || !bOnlyWinning)) {
          if (minus_streak > MAX_STREAK) {
            printf("minus_streak (%d) > MAX_STREAK (%d) on lines %d\n",
              minus_streak,MAX_STREAK,line_no);
            return 7;
          }

          streak_histogram[minus_streak - 1]++;
        }

        if (!bOnlyWinning && !bMaxPlus && !bMaxMinus) {
          if (!bVerbose) {
            if (!bOnlyLosing) {
              if (minus_streak >= ge_value)
                printf("-%d\n",minus_streak);
            }
            else {
              if (minus_streak >= ge_value)
                printf("%d\n",minus_streak);
            }
          }
          else {
            if (!bOnlyLosing) {
              if (minus_streak >= ge_value)
                printf("-%d %s %s\n",minus_streak,save_line0,save_line);
            }
            else {
              if (minus_streak >= ge_value)
                printf("%d %s %s\n",minus_streak,save_line0,save_line);
            }
          }
        }

        if (minus_streak > max_minus_streak)
          max_minus_streak = minus_streak;

        minus_streak = 0;
        plus_streak = 1;
      }
    }

    if (bVerbose) {
      if (minus_streak + plus_streak == 1)
        strcpy(save_line0,line);

      strcpy(save_line,line);
    }
  }

  fclose(fptr);

  if (plus_streak > max_plus_streak)
    max_plus_streak = plus_streak;
  else if (minus_streak > max_minus_streak)
    max_minus_streak = minus_streak;

  if (!bMaxPlus && !bMaxMinus) {
    if (plus_streak >= 1) {
      if (!bOnlyLosing) {
        if (!bVerbose) {
          if (!bOnlyWinning)
            printf("+%d\n",plus_streak);
          else
            printf("%d\n",plus_streak);
        }
        else {
          if (!bOnlyWinning)
            printf("+%d %s %s\n",plus_streak,save_line0,save_line);
          else
            printf("%d %s %s\n",plus_streak,save_line0,save_line);
        }
      }
    }
    else if (minus_streak >= 1) {
      if (!bOnlyWinning) {
        if (!bVerbose) {
          if (!bOnlyLosing)
            printf("-%d\n",minus_streak);
          else
            printf("%d\n",minus_streak);
        }
        else {
          if (!bOnlyLosing)
            printf("-%d %s %s\n",minus_streak,save_line0,save_line);
          else
            printf("%d %s %s\n",minus_streak,save_line0,save_line);
        }
      }
    }
  }

  if (bMaxPlus)
    printf("%d %s\n",max_plus_streak,save_dir);
  else if (bMaxMinus)
    printf("%d %s\n",max_minus_streak,save_dir);
  else if (bVerbose) {
    putchar(0x0a);
    printf("nobs:             %6d\n",line_no);
    printf("max_plus_streak:  %6d\n",max_plus_streak);
    printf("max_minus_streak: %6d\n",max_minus_streak);
  }

  if (bHistogram) {
    printf("\nhistogram:\n\n");

    for (n = MAX_STREAK - 1; (n >= 0); n--) {
      if (streak_histogram[n])
        printf("%3d: %5d\n",n+1,streak_histogram[n]);
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
