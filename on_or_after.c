#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define BUF_SIZE 4096
static char buf[BUF_SIZE];

static char usage[] =
"usage: on_or_after (-not) filename (filename filename ...)\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bNot;
  bool bStdin;
  char *filename;
  time_t on_or_after;
  struct stat statbuf;
  char *cpt;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bNot = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else
      break;
  }

  if (!stat(argv[curr_arg],&statbuf))
    on_or_after = statbuf.st_mtime;
  else {
    printf("couldn't get status of %s\n",argv[curr_arg]);
    return 2;
  }

  curr_arg++;

  if (curr_arg == argc) {
    filename = buf;
    bStdin = true;
  }
  else
    bStdin = false;

  for ( ; ; ) {
    if (bStdin) {
      fscanf(stdin,"%s",buf);

      if (feof(stdin))
        break;
    }
    else
      filename = argv[curr_arg];

    if (!stat(filename,&statbuf)) {
      if (!bNot) {
        if (statbuf.st_mtime >= on_or_after) {
          cpt = ctime(&statbuf.st_mtime);
          cpt[strlen(cpt) - 1] = 0;

          printf("%s %s\n",cpt,filename);
        }
      }
      else {
        if (statbuf.st_mtime < on_or_after) {
          cpt = ctime(&statbuf.st_mtime);
          cpt[strlen(cpt) - 1] = 0;

          printf("%s %s\n",cpt,filename);
        }
      }
    }

    if (!bStdin) {
      curr_arg++;

      if (curr_arg == argc)
        break;
    }
  }

  return 0;
}
