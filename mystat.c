#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static char usage[] = "usage: mystat filename (filename ...)\n";
static char status_err[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (n = 1; n < argc; n++) {
    if (stat(argv[n],&statbuf))
      printf("couldn't get status of %s\n",argv[n]);
  }

  return 0;
}
