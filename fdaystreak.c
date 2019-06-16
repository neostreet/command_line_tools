#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char streak_start[MAX_LINE_LEN];
static char streak_end[MAX_LINE_LEN];

static int year_ix;
static int month_ix;
static int day_ix;

static char usage[] =
"usage: fdaystreak (-debug) (-mdy) (-reverse) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char invalid_date[] = "invalid date on line %d\n";

static char *months[] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};
#define NUM_MONTHS (sizeof months / sizeof (char *))

#define SECS_PER_MIN  (double)60
#define MINS_PER_HOUR (double)60
#define HOURS_PER_DAY (double)24
#define DAYS_PER_WEEK (double)7
#define DAYS_PER_YEAR (double)365.25
#define SECS_PER_DAY (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY)
#define SECS_PER_WEEK (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY * DAYS_PER_WEEK)
#define SECS_PER_YEAR (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY * DAYS_PER_YEAR)

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static time_t cvt_date(char *date_str);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bMdy;
  bool bReverse;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  time_t date1;
  time_t date2;
  time_t datediff;
  double dwork;
  int work;
  char *cpt;
  int curr_streak;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bMdy = false;
  bReverse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-mdy"))
      bMdy = true;
    else if (!strcmp(argv[curr_arg],"-reverse"))
      bReverse = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  if (!bMdy) {
    year_ix = 0;
    month_ix = 1;
    day_ix = 2;
  }
  else {
    month_ix = 0;
    day_ix = 1;
    year_ix = 2;
  }

  date_checks[year_ix].lower = 80;
  date_checks[year_ix].upper = 2095;
  date_checks[month_ix].lower = 1;
  date_checks[month_ix].upper = 12;
  date_checks[day_ix].lower = 1;
  date_checks[day_ix].upper = 31;

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    date2 = cvt_date(line);

    if (date2 == -1L) {
      printf(invalid_date,line_no);
      return 5;
    }

    if (line_no == 1) {
      date1 = date2;

      if (!bReverse) {
        curr_streak = 1;
        strcpy(streak_start,line);
        strcpy(streak_end,line);
      }

      continue;
    }

    if (date1 > date2)
      datediff = date1 - date2;
    else
      datediff = date2 - date1;

    dwork = (double)datediff / (double)SECS_PER_DAY;
    work = (int)dwork;

    if (work != 1) {
      if (!bReverse) {
        printf("%d %s %s\n",curr_streak,streak_start,streak_end);
        curr_streak = 1;
        strcpy(streak_start,line);
        strcpy(streak_end,line);
      }
      else
        printf("%d %s\n",work - 1,line);
    }
    else {
      if (!bReverse) {
        curr_streak++;
        strcpy(streak_end,line);
      }
    }

    date1 = date2;
  }

  if (!bReverse)
    printf("%d %s %s\n",curr_streak,streak_start,streak_end);

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

static time_t cvt_date(char *date_str)
{
  struct tm tim;
  char hold[11];
  int date_len;
  int bufix;
  int holdix;
  int digits[3];
  int n;
  time_t ret_tm;

  date_len = strlen(date_str);

  if (!date_len || (date_len > 10))
    return -1L;

  bufix = 0;

  for (n = 0; n < 3; n++) {
    holdix = 0;

    for ( ; bufix < date_len; ) {
      if (date_str[bufix] == '-') {
        bufix++;
        break;
      }

      if ((date_str[bufix] < '0') || (date_str[bufix] > '9'))
        return -1L;

      hold[holdix++] = date_str[bufix++];
    }

    if (!holdix || ((n != 2) && (bufix == date_len)))
      return -1L;

    hold[holdix] = 0;
    digits[n] = atoi(hold);

    if ((digits[n] > date_checks[n].upper) ||
      (digits[n] < date_checks[n].lower))
      return -1L;
  }

  if (digits[year_ix] >= 100)
    if (digits[year_ix] < 1970)
      return -1L;
    else
      digits[year_ix] -= 1900;

  tim.tm_mon = digits[month_ix] - 1;
  tim.tm_mday = digits[day_ix];
  tim.tm_year = digits[year_ix];

  tim.tm_hour = 0;
  tim.tm_min = 0;
  tim.tm_sec = 0;

  tim.tm_isdst = 0;

  ret_tm = mktime(&tim);

  return ret_tm;
}
