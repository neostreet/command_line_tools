#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

#define MONTH_IX 0
#define DAY_IX   1
#define YEAR_IX  2

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

#define SECONDS_IN_MINUTE 60
#define MINUTES_IN_HOUR 60
#define HOURS_IN_DAY 24
#define SECONDS_IN_DAY (SECONDS_IN_MINUTE * MINUTES_IN_HOUR * HOURS_IN_DAY)

#define BUF_SIZE 4096
static char buf[BUF_SIZE];

static char usage[] = "usage: on_date date (filename filename ...)\n";

static time_t cvt_date(char *date_str);

int main(int argc,char **argv)
{
  int n;
  int bStdin;
  char *filename;
  time_t on_date;
  time_t compare_date;
  struct stat statbuf;
  char *cpt;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  on_date = cvt_date(argv[1]);

  if (on_date == -1L) {
    printf("%s: invalid date\n",argv[1]);
    return 2;
  }

  on_date /= SECONDS_IN_DAY;
  on_date *= SECONDS_IN_DAY;

  if (argc == 2) {
    filename = buf;
    bStdin = TRUE;
  }
  else {
    n = 2;
    bStdin = FALSE;
  }

  for ( ; ; ) {
    if (bStdin) {
      fscanf(stdin,"%s",buf);

      if (feof(stdin))
        break;
    }
    else
      filename = argv[n];

    if (!stat(filename,&statbuf)) {
      compare_date = statbuf.st_mtime;
      compare_date /= SECONDS_IN_DAY;
      compare_date *= SECONDS_IN_DAY;

      if (compare_date == on_date) {
        cpt = ctime(&statbuf.st_mtime);
        cpt[strlen(cpt) - 1] = 0;

        printf("%s %s\n",cpt,filename);
      }
    }

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  return 0;
}

static time_t cvt_date(char *date_str)
{
  time_t date1;
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
