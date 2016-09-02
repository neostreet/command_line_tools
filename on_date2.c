#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

#define SECONDS_IN_MINUTE 60
#define MINUTES_IN_HOUR 60
#define HOURS_IN_DAY 24
#define SECONDS_IN_DAY (SECONDS_IN_MINUTE * MINUTES_IN_HOUR * HOURS_IN_DAY)

#define BUF_SIZE 4096
static char buf[BUF_SIZE];

static char usage[] = "usage: on_date2 filename (filename filename ...)\n";

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

  if (!stat(argv[1],&statbuf)) {
    on_date = statbuf.st_mtime;
    on_date /= SECONDS_IN_DAY;
    on_date *= SECONDS_IN_DAY;
  }
  else {
    printf("couldn't get status of %s\n",argv[1]);
    return 2;
  }

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
