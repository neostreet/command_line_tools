#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

static char usage[] = "usage: dateadd date days\n";

#define SECS_PER_MIN  60
#define MINS_PER_HOUR 60
#define HOURS_PER_DAY 24
#define SECS_PER_DAY (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY)

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

static time_t cvt_date(char *date_str);

int main(int argc,char **argv)
{
  int retval;
  time_t date1;
  int days;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  date1 = cvt_date(argv[1]);

  if (date1 == -1L) {
    printf("%s: invalid date\n",argv[1]);
    return 2;
  }

  sscanf(argv[2],"%d",&days);

  date1 += days * SECS_PER_DAY;

  printf("%s",ctime(&date1));

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

  if (digits[2] >= 100)
    if (digits[2] < 1970)
      return -1L;
    else
      digits[2] -= 1900;

  tim.tm_mon = digits[0] - 1;
  tim.tm_mday = digits[1];
  tim.tm_year = digits[2];

  tim.tm_hour = 0;
  tim.tm_min = 0;
  tim.tm_sec = 0;

  tim.tm_isdst = 0;

  ret_tm = mktime(&tim);

  return ret_tm;
}
