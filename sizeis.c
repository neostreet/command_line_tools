#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FALSE 0
#define TRUE  1

#define MAX_FILENAME_LEN 512
static char filename[MAX_FILENAME_LEN];

int main(int argc,char **argv)
{
  int n;
  off_t size;
  struct stat statbuf;
  int bStdin;
  char *filenam;

  if (argc < 2) {
    printf("usage: sizeis size (filename filename ...)\n");
    return 1;
  }

  sscanf(argv[1],"%d",&size);

  if (argc == 2) {
    bStdin = TRUE;
    filenam = filename;
  }
  else {
    bStdin = FALSE;
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
      if (statbuf.st_size == size)
        printf("%s\n",filenam);

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  return 0;
}
