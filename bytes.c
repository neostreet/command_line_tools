#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

static char usage[] =
"usage: bytes (-debug) filename (filename ...)\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  struct stat stat_buf;
  int total_bytes;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  total_bytes = 0;

  for ( ; curr_arg < argc; curr_arg++) {
    if (stat(argv[curr_arg],&stat_buf) != -1)
      total_bytes += stat_buf.st_size;
    else
      printf("stat() failed on %s\n",argv[curr_arg]);
  }

  printf("%d\n",total_bytes);

  return 0;
}
