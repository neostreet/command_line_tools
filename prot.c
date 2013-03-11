#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

int main(int argc,char **argv)
{
  int n;
  bool bStdin;
  char *filenam;
  struct stat statbuf;
  mode_t mode;

  if (argc == 1) {
    bStdin = true;
    filenam = filename;
  }
  else {
    bStdin = false;
    n = 1;
  }

  for ( ; ; ) {
    if (bStdin) {
      fscanf(stdin,"%s",filename);

      if (feof(stdin))
        break;
    }
    else
      filenam = argv[n];

    if (!stat(filenam,&statbuf)) {
      mode = statbuf.st_mode;

      if (!(mode & S_IWGRP) && !(mode & S_IWOTH))
        printf("%s\n",filenam);
    }

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  return 0;
}
