#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE  1

static char usage[] = "usage: mysql_grab (-to_screen) script\n";

#define BUFLEN 256
static char buf[BUFLEN];

int main(int argc,char **argv)
{
  int curr_arg;
  int bToScreen;

  if ((argc != 2) && (argc != 3)) {
    printf(usage);
    return 1;
  }

  bToScreen = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-to_screen"))
      bToScreen = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (!bToScreen) {
    sprintf(buf,"mysql -uaidan -ppoirmarp < %s.sql > %s.out",
      argv[curr_arg],argv[curr_arg]);
  }
  else {
    sprintf(buf,"mysql -uaidan -ppoirmarp < %s.sql",
      argv[curr_arg]);
  }

  system(buf);
  system("mydir /o-d *.out > out.lst");

  return 0;
}
