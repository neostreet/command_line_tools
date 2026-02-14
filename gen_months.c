#include <stdio.h>

static char *months[] = {
  "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"
};
#define NUM_MONTHS (sizeof (months) / sizeof (char *))

int main(int argc,char **argv)
{
  int m;
  int n;
  int start_year;
  int start_month;
  int end_year;
  int end_month;
  int curr_start_month;
  int curr_end_month;

  if (argc != 6) {
    printf("usage: gen_months start_year start_month end_year end_month suffix\n");
    return 1;
  }

  sscanf(argv[1],"%d",&start_year);
  sscanf(argv[2],"%d",&start_month);
  sscanf(argv[3],"%d",&end_year);
  sscanf(argv[4],"%d",&end_month);

  for (m = start_year; m <= end_year; m++) {
    if (m == start_year) {
      curr_start_month = start_month - 1;

      if (m == end_year)
        curr_end_month = end_month - 1;
      else
        curr_end_month = NUM_MONTHS - 1;
    }
    else if (m == end_year) {
      curr_start_month = 0;
      curr_end_month = end_month - 1;
    }
    else {
      curr_start_month = 0;
      curr_end_month = NUM_MONTHS - 1;
    }

    for (n = curr_start_month; n <= curr_end_month; n++)
      printf("%d%s%s\n",m,months[n],argv[5]);
  }

  return 0;
}
