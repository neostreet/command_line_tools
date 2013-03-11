#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_FILENAME_LEN 512
static char filename[MAX_FILENAME_LEN];

int main(int argc,char **argv)
{
  int n;
  off_t size;
  struct stat statbuf;
  bool bStdin;
  char *filenam;

  if (argc < 2) {
    printf("usage: sizeis filename (filename filename ...)\n");
    return 1;
  }

  if (!stat(argv[1],&statbuf))
    size = statbuf.st_size;
  else {
    printf("couldn't get status of %s\n",argv[1]);
    return 2;
  }

  if (argc == 2) {
    bStdin = true;
    filenam = filename;
  }
  else {
    bStdin = false;
    n = 2;
  }

  for ( ; ; ) {
    if (bStdin) {
      fscanf(stdin,"%s",filename);

      if (feof(stdin))
        break;
    }
    else
      filenam = argv[n];

    if (!stat(filenam,&statbuf))
      if ((statbuf.st_size == size) && (strcmp(filenam,argv[1])))
        printf("%s\n",filenam);

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  return 0;
}
