#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

static char usage[] =
"usage: timeis_like (-date_only) filename (filename filename ...)\n";

void get_year_month_day(time_t *timtpt,int *yearpt,int *monthpt,int *daypt);

#define MAX_FILENAME_LEN 512
static char filename[MAX_FILENAME_LEN];

static char *months[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#define NUM_MONTHS (sizeof months / sizeof (char *))

int main(int argc,char **argv)
{
  int curr_arg;
  int like_file_arg;
  bool bDateOnly;
  time_t mtime;
  int year;
  int month;
  int day;
  int compare_year;
  int compare_month;
  int compare_day;
  struct stat statbuf;
  int bStdin;
  char *filenam;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bDateOnly = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-date_only"))
      bDateOnly = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  like_file_arg = curr_arg;

  if (!stat(argv[like_file_arg],&statbuf)) {
    mtime = statbuf.st_mtime;

    if (bDateOnly)
      get_year_month_day(&mtime,&year,&month,&day);
  }
  else {
    printf("couldn't get status of %s\n",argv[like_file_arg]);
    return 3;
  }

  if (argc - curr_arg == 1) {
    bStdin = true;
    filenam = filename;
  }
  else {
    bStdin = false;
    curr_arg++;
  }

  for ( ; ; ) {
    if (bStdin) {
      fscanf(stdin,"%s",filename);

      if (feof(stdin))
        break;
    }
    else
      filenam = argv[curr_arg];

    if ((strcmp(filenam,argv[like_file_arg])) && !stat(filenam,&statbuf)) {
      if (!bDateOnly) {
        if ((statbuf.st_mtime == mtime))
          printf("%s\n",filenam);
      }
      else {
        get_year_month_day(&statbuf.st_mtime,&compare_year,&compare_month,&compare_day);

        if ((compare_year == year) && (compare_month == month) && (compare_day == day))
          printf("%s\n",filenam);
      }
    }

    if (!bStdin) {
      curr_arg++;

      if (curr_arg == argc)
        break;
    }
  }

  return 0;
}

void get_year_month_day(time_t *timtpt,int *yearpt,int *monthpt,int *daypt)
{
  int m;
  char *cpt;
  int month;

  cpt = ctime(timtpt);

  sscanf(&cpt[20],"%d",yearpt);

  for (month = 0; month < NUM_MONTHS; month++) {
    for (m = 0; m < 3; m++) {
      if (months[month][m] != cpt[4+m])
        break;
    }

    if (m == 3)
      break;
  }

  if (month == NUM_MONTHS)
    month = 1;
  else
    month++;

  *monthpt = month;

  sscanf(&cpt[8],"%d",daypt);
}
