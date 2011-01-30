#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

static char usage[] = "usage: whatday date (date ...)\n";

static int cvt_date(char *date_str,time_t *timt);

int main(int argc,char **argv)
{
  int n;
  time_t timt;
  int retval;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (n = 1; n < argc; n++) {
    retval = cvt_date(argv[n],&timt);

    if (retval)
      printf("invalid date: %d\n",retval);
    else
      printf("%s",ctime(&timt));
  }

  return 0;
}

static int cvt_date(char *date_str,time_t *timt)
{
  struct tm tim;
  char hold[11];
  int date_len;
  int bufix;
  int holdix;
  int digits[3];
  int n;

  date_len = strlen(date_str);

  if (!date_len || (date_len > 10))
    return 1;

  bufix = 0;

  for (n = 0; n < 3; n++) {
    holdix = 0;

    for ( ; bufix < date_len; ) {
      if (date_str[bufix] == '-') {
        bufix++;
        break;
      }

      if ((date_str[bufix] < '0') || (date_str[bufix] > '9'))
        return 2;

      hold[holdix++] = date_str[bufix++];
    }

    if (!holdix || ((n != 2) && (bufix == date_len)))
      return 3;

    if ((n == 2) && (holdix != 4))
      return 4;

    hold[holdix] = 0;
    digits[n] = atoi(hold);

    if ((digits[n] > date_checks[n].upper) ||
      (digits[n] < date_checks[n].lower))
      return 5;
  }

  tim.tm_mon = digits[0] - 1;
  tim.tm_mday = digits[1];
  tim.tm_year = digits[2] - 1900;

  tim.tm_hour = 0;
  tim.tm_min = 0;
  tim.tm_sec = 0;

  tim.tm_isdst = 0;

  *timt = mktime(&tim);

  if (*timt == -1)
    return 6;

  return 0;
}
