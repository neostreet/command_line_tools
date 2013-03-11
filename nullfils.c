#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static char usage[] = "usage: nullfils (-not) filename (filename ...)\n";

int main(int argc,char **argv)
{
  int n;
  bool bNot;
  struct stat statbuf;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bNot = false;

  if (!strcmp(argv[1],"-not"))
    bNot = true;

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
