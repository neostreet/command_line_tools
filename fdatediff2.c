#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

enum enumUnit {
  UNIT_SECS,
  UNIT_DAYS,
  UNIT_WEEKS,
  UNIT_YEARS
};

static char usage[] =
"usage: fdatediff2 (-debug) (-year_first) [secs | days | weeks | years] filename\n";
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
static time_t cvt_date(char *date_str,int bYearFirst);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bYearFirst;
  enumUnit un;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  time_t date1;
  time_t date2;
  time_t datediff;
  double dwork;
  char *cpt;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bYearFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-year_first"))
      bYearFirst = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg],"secs"))
    un = UNIT_SECS;
  else if (!strcmp(argv[curr_arg],"days"))
    un = UNIT_DAYS;
  else if (!strcmp(argv[curr_arg],"weeks"))
    un = UNIT_WEEKS;
  else if (!strcmp(argv[curr_arg],"years"))
    un = UNIT_YEARS;
  else {
    printf("invalid unit\n");
    return 3;
  }

  curr_arg++;

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

    for (n = 0; n < line_len; n++) {
      if (line[n] == ' ')
        break;
    }

    if (n < line_len)
      line[n] = 0;

    date2 = cvt_date(line,bYearFirst);

    if (date2 == -1L) {
      printf(invalid_date,line_no);
      return 5;
    }

    if (line_no == 1) {
      date1 = date2;
      continue;
    }

    if (date1 > date2)
      datediff = date1 - date2;
    else
      datediff = date2 - date1;

    switch(un) {
      case UNIT_SECS:
        printf("%ld seconds\n",datediff);
        break;
      case UNIT_DAYS:
        dwork = (double)datediff / (double)SECS_PER_DAY;
        printf("%lf days\n",dwork);
        break;
      case UNIT_WEEKS:
        dwork = (double)datediff / (double)SECS_PER_WEEK;
        printf("%lf weeks\n",dwork);
        break;
      case UNIT_YEARS:
        dwork = (double)datediff / (double)SECS_PER_YEAR;
        printf("%lf years\n",dwork);
        break;
    }

    date1 = date2;
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

static time_t cvt_date(char *date_str,int bYearFirst)
{
  int month_ix;
  int day_ix;
  int year_ix;
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

  if (!bYearFirst) {
    month_ix = 0;
    day_ix = 1;
    year_ix = 2;
  }
  else {
    year_ix = 0;
    month_ix = 1;
    day_ix = 2;
  }

  date_checks[month_ix].lower = 1;
  date_checks[month_ix].upper = 12;
  date_checks[day_ix].lower = 1;
  date_checks[day_ix].upper = 31;
  date_checks[year_ix].lower = 80;
  date_checks[year_ix].upper = 2095;

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
