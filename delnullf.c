#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

static char usage[] = "usage: delnullf filename (filename ...)\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  for (n = 1; n < argc; n++) {
    if (!stat(argv[n],&statbuf)) {
      if (!(statbuf.st_mode & S_IFDIR) && !statbuf.st_size) {
        if (remove(argv[n]))
          printf("couldn't remove %s\n",argv[n]);
      }
    }
  }

  return 1;
}
