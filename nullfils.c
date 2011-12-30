#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FALSE 0
#define TRUE  1

static char usage[] = "usage: nullfils (-not) filename (filename ...)\n";

int main(int argc,char **argv)
{
  int n;
  int bNot;
  struct stat statbuf;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bNot = FALSE;

  if (!strcmp(argv[1],"-not"))
    bNot = TRUE;

  if (argc - bNot < 2) {
    printf(usage);
    return 2;
  }

  for (n = 1 + bNot; n < argc; n++) {
    if (!stat(argv[n],&statbuf)) {
      if (!(statbuf.st_mode & S_IFDIR)) {
        if (!bNot && !statbuf.st_size)
          printf("%s\n",argv[n]);
        else if (bNot && statbuf.st_size)
          printf("%s\n",argv[n]);
      }
    }
  }

  return 0;
}
