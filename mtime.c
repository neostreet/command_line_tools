#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define BUF_SIZE 4096
static char buf[BUF_SIZE];

int main(int argc,char **argv)
{
  int n;
  int work_len;
  struct stat statbuf;
  char *cpt;
  bool bStdin;
  char *filename;
  bool bHex;

  bHex = false;

  if (argc == 1) {
    filename = buf;
    bStdin = true;
  }
  else {
    if (!strcmp(argv[1],"-hex")) {
      bHex = true;

      if (argc > 2) {
        n = 2;
        bStdin = false;
      }
      else {
        filename = buf;
        bStdin = true;
      }
    }
    else {
      n = 1;
      bStdin = false;
    }
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
      if (!bHex) {
        cpt = ctime(&statbuf.st_mtime);
        cpt[strlen(cpt) - 1] = 0;

        printf("%s %10d %s\n",cpt,
          statbuf.st_size,filename);
      }
      else
        printf("%08x %10d %s\n",
          statbuf.st_mtime,
          statbuf.st_size,filename);
    }

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  return 0;
}
