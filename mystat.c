#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

static char usage[] =
"usage: mystat (-verbose) (-reverse) filename (filename ...)\n";
static char status_err[] = "couldn't get status of %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bReverse;
  struct stat statbuf;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bReverse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-reverse"))
      bReverse = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  if (bVerbose)
    getcwd(save_dir,_MAX_PATH);

  for ( ; curr_arg < argc; curr_arg++) {
    if (!bReverse) {
      if (stat(argv[curr_arg],&statbuf)) {
        if (!bVerbose)
          printf("couldn't get status of %s\n",argv[curr_arg]);
        else
          printf("couldn't get status of %s/%s\n",save_dir,argv[curr_arg]);
      }
    }
    else {
      if (stat(argv[curr_arg],&statbuf))
        ;
      else {
        if (!bVerbose)
          printf("%s\n",argv[curr_arg]);
        else
          printf("%s/%s\n",save_dir,argv[curr_arg]);
      }
    }
  }

  return 0;
}
