#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define YEAR_IX  0
#define MONTH_IX 1
#define DAY_IX   2

static char usage[] = "usage: timediff date time date time\n";

static char couldnt_get_status[] = "couldn't get status of %s\n";
static char invalid_date_and_time[] = "%s %s: invalid date and time\n";

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  80, 2095,  /* year */
  1, 12,     /* month */
  1, 31      /* day */
};

static struct digit_range time_checks[3] = {
  0, 23,  /* hour */
  0, 59,     /* minute */
  0, 59      /* second */
};

static time_t cvt_date_and_time(char *date_str,char *time_str);

int main(int argc,char **argv)
{
  time_t date1;
  time_t date2;
  int diff;
  int hours;

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  date1 = cvt_date_and_time(argv[1],argv[2]);

  if (date1 == -1L) {
    printf(invalid_date_and_time,argv[1],argv[2]);
    return 2;
  }

  date2 = cvt_date_and_time(argv[3],argv[4]);

  if (date2 == -1L) {
    printf(invalid_date_and_time,argv[3],argv[4]);
    return 3;
  }

  diff = date2 - date1;

  if (diff < 60)
    printf("%d seconds\n",diff);
  else if (diff < 60 * 60)
    printf("%d minutes, %d seconds\n",diff / 60,diff % 60);
  else {
    hours = diff / (60 * 60);
    diff -= (hours * 60 * 60);
    printf("%d hours, %d minutes, %d seconds\n",hours,diff / 60,diff %60);
  }

  return 0;
}

static time_t cvt_date_and_time(char *date_str,char *time_str)
{
  struct tm tim;
  char hold[11];
  int date_len;
  int time_len;
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
      if (date_str[bufix] == '/') {
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

  time_len = strlen(time_str);

  if (!time_len || (time_len > 8))
    return -1L;

  bufix = 0;

  for (n = 0; n < 3; n++) {
    holdix = 0;

    for ( ; bufix < time_len; ) {
      if (time_str[bufix] == ':') {
        bufix++;
        break;
      }

      if ((time_str[bufix] < '0') || (time_str[bufix] > '9'))
        return -1L;

      hold[holdix++] = time_str[bufix++];
    }

    if (!holdix || ((n != 2) && (bufix == time_len)))
      return -1L;

    hold[holdix] = 0;
    digits[n] = atoi(hold);

    if ((digits[n] > time_checks[n].upper) ||
      (digits[n] < time_checks[n].lower))
      return -1L;
  }

  tim.tm_hour = digits[0];
  tim.tm_min = digits[1];
  tim.tm_sec = digits[2];

  tim.tm_isdst = 0;

  ret_tm = mktime(&tim);

  return ret_tm;
}
