#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

#define MONTH_IX 0
#define DAY_IX   1
#define YEAR_IX  2

static char usage[] =
"usage: fdaystreak (-debug) filename\n";
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

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static time_t cvt_date(char *date_str);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
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

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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
      curr_streak = 1;
      strcpy(save_line,line);
      continue;
    }

    if (date1 > date2)
      datediff = date1 - date2;
    else
      datediff = date2 - date1;

    dwork = (double)datediff / (double)SECS_PER_DAY;
    work = (int)dwork;

    if (work != 1) {
      printf("%d %s\n",curr_streak,save_line);
      curr_streak = 1;
      strcpy(save_line,line);
    }
    else
      curr_streak++;

    date1 = date2;
  }

  printf("%d %s\n",curr_streak,save_line);

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

  if (digits[YEAR_IX] >= 100)
    if (digits[YEAR_IX] < 1970)
      return -1L;
    else
      digits[YEAR_IX] -= 1900;

  tim.tm_mon = digits[MONTH_IX] - 1;
  tim.tm_mday = digits[DAY_IX];
  tim.tm_year = digits[YEAR_IX];

  tim.tm_hour = 0;
  tim.tm_min = 0;
  tim.tm_sec = 0;

  tim.tm_isdst = 0;

  ret_tm = mktime(&tim);

  return ret_tm;
}
