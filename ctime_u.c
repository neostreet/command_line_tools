#include <stdio.h>
#include <string.h>
#include <time.h>

int main(int argc,char **argv)
{
  int n;
  time_t tim;

  if (argc < 2) {
    printf("usage: ctime_u time (time ...)\n");
    return 1;
  }

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"now"))
      time(&tim);
    else
      sscanf(argv[n],"%lu",&tim);

    printf("%lx %lu %s",tim,tim,ctime(&tim));
  }

  return 0;
}
