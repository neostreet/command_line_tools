#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define SECS_PER_MIN  60
#define MINS_PER_HOUR 60
#define HOURS_PER_DAY 24
#define SECS_PER_DAY (SECS_PER_MIN * MINS_PER_HOUR * HOURS_PER_DAY)

static char usage[] = "usage: span filename (filename ...)\n";
static char fmt_str[] = "%s %10d %s\n";

int main(int argc,char **argv)
{
  int n;
  struct stat stat_buf;
  int earliest_ix;
  time_t earliest_mtime;
  int earliest_st_size;
  int latest_ix;
  time_t latest_mtime;
  int latest_st_size;
  char *cpt;
  time_t datediff;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  earliest_ix = -1;

  for (n = 1; n < argc; n++) {
    if (stat(argv[n],&stat_buf) != -1) {
      if (earliest_ix == -1) {
        earliest_ix = n;
        earliest_mtime = stat_buf.st_mtime;
        earliest_st_size = stat_buf.st_size;
        latest_ix = n;
        latest_mtime = stat_buf.st_mtime;
        latest_st_size = stat_buf.st_size;
      }
      else if (stat_buf.st_mtime < earliest_mtime) {
        earliest_ix = n;
        earliest_mtime = stat_buf.st_mtime;
        earliest_st_size = stat_buf.st_size;
      }
      else if (stat_buf.st_mtime > latest_mtime) {
        latest_ix = n;
        latest_mtime = stat_buf.st_mtime;
        latest_st_size = stat_buf.st_size;
      }
    }
  }

  if (earliest_ix != -1) {
    cpt = ctime(&earliest_mtime);
    cpt[strlen(cpt) - 1] = 0;
    printf(fmt_str,cpt,
      earliest_st_size,argv[earliest_ix]);

    cpt = ctime(&latest_mtime);
    cpt[strlen(cpt) - 1] = 0;
    printf(fmt_str,cpt,
      latest_st_size,argv[latest_ix]);

    datediff = latest_mtime - earliest_mtime;
    datediff /= (SECS_PER_DAY);

    printf("\n%d days\n",datediff);
  }

  return 0;
}
