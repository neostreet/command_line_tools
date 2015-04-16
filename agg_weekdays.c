#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define MONTH_IX 0
#define DAY_IX   1
#define YEAR_IX  2

static char usage[] =
"usage: agg_weekdays (-debug) (-runtot) date1 date2\n";

static char *months[] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};
#define NUM_MONTHS (sizeof months / sizeof (char *))

#define SECS_PER_MIN  (double)60
#define MINS_PER_HOUR (double)60
#define HOURS_PER_DAY (double)24
#define SECS_PER_DAY (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY)

static char *weekdays[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
#define NUM_WEEKDAYS (sizeof weekdays / sizeof (char *))

static char couldnt_get_status[] = "couldn't get status of %s\n";

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

static int get_today(time_t *today_ptr);
static int get_month(char *month_str,int *month_ptr);
static time_t cvt_date(char *date_str);
static int get_weekday(char *weekday_str,int *weekday_ptr);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bRuntot;
  int retval;
  time_t today;
  time_t date1;
  time_t date2;
  char *cpt;
  int weekday_counts[NUM_WEEKDAYS];
  int weekday;
  int total_days;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bRuntot= false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-runtot"))
      bRuntot = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  retval = get_today(&today);

  if (retval) {
    printf("get_today() failed: %d\n",retval);
    return 3;
  }

  if (!strcmp(argv[curr_arg],"today"))
    date1 = today;
  else
    date1 = cvt_date(argv[curr_arg]);

  if (date1 == -1L) {
    printf("%s: invalid date\n",argv[curr_arg]);
    return 4;
  }

  if (!strcmp(argv[curr_arg+1],"today"))
    date2 = today;
  else
    date2 = cvt_date(argv[curr_arg+1]);

  if (date2 == -1L) {
    printf("%s: invalid date\n",argv[curr_arg+1]);
    return 5;
  }

  if (bDebug) {
    cpt = ctime(&date1);
    cpt[strlen(cpt)-1] = 0;
    printf("date1: %s\n",cpt);
    cpt = ctime(&date2);
    cpt[strlen(cpt)-1] = 0;
    printf("date2: %s\n",cpt);
  }

  if (date1 >= date2) {
    printf("date1 must be less than date2\n");
    return 6;
  }

  for (n = 0; n < NUM_WEEKDAYS; n++)
    weekday_counts[n] = 0;

  for ( ; date1 < date2; date1 += SECS_PER_DAY) {
    cpt = ctime(&date1);

    retval = get_weekday(&cpt[0],&weekday);

    if (retval) {
      printf("get_weekday_failed: %d\n",retval);
      return 7;
    }

    weekday_counts[weekday]++;
  }

  total_days = 0;

  for (n = 0; n < NUM_WEEKDAYS; n++) {
    total_days += weekday_counts[n];

    if (!bRuntot)
      printf("%7d %s\n",weekday_counts[n],weekdays[n]);
    else
      printf("%7d %s %7d\n",weekday_counts[n],weekdays[n],total_days);
  }

  if (!bRuntot)
    printf("\n%7d\n",total_days);

  return 0;
}

static int get_today(time_t *today_ptr)
{
  int n;
  time_t now;
  char *cpt;
  int retval;
  int month;
  char today_buf[11];
  time_t today;

  time(&now);
  cpt = ctime(&now);

  retval = get_month(&cpt[4],&month);

  if (retval)
    return 1;

  sprintf(today_buf,"%02d-",month);

  for (n = 0; n < 2; n++)
    today_buf[3+n] = cpt[8+n];

  if (today_buf[3] == ' ')
    today_buf[3] = '0';

  today_buf[5] = '-';

  for (n = 0; n < 4; n++)
    today_buf[6+n] = cpt[20+n];

  today_buf[10] = 0;

  today = cvt_date(today_buf);

  if (today == -1L)
    return 2;

  *today_ptr = today;

  return 0;
}

static int get_month(char *month_str,int *month_ptr)
{
  int n;
  int month;

  *month_ptr = 0;

  for (month = 0; month < NUM_MONTHS; month++) {
    for (n = 0; n < 3; n++) {
      if (toupper(month_str[n]) != months[month][n])
        break;
    }

    if (n == 3)
      break;
  }

  if (month == NUM_MONTHS)
    return 1;

  *month_ptr = month + 1;
  return 0;
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

static int get_weekday(char *weekday_str,int *weekday_ptr)
{
  int n;
  int weekday;

  *weekday_ptr = 0;

  for (weekday = 0; weekday < NUM_WEEKDAYS; weekday++) {
    for (n = 0; n < 3; n++) {
      if (weekday_str[n] != weekdays[weekday][n])
        break;
    }

    if (n == 3)
      break;
  }

  if (weekday == NUM_WEEKDAYS)
    return 1;

  *weekday_ptr = weekday;
  return 0;
}
