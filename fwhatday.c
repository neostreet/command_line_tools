#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

struct digit_range {
  int lower;
  int upper;
};

static struct digit_range date_checks[3] = {
  1, 12,     /* month */
  1, 31,     /* day */
  80, 2095   /* year */
};

static char usage[] = "usage: fwhatday file\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int cvt_date(char *date_str,time_t *timt);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  time_t timt;
  int retval;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    retval = cvt_date(line,&timt);

    if (retval)
      printf("invalid date: %d\n",retval);
    else
      printf("%s",ctime(&timt));
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

int cvt_date(char *date_str,time_t *timt)
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
