#include <stdio.h>

int main(int argc,char **argv)
{
  int n;
  int integer;
  int distance;
  int work;

  if (argc != 3) {
    printf("usage: sumtorial integer distance\n");
    return 1;
  }

  sscanf(argv[1],"%d",&integer);
  sscanf(argv[2],"%d",&distance);

  work = 0;

  do {
    work += integer;

    integer -= distance;
  } while (integer > 0);

  printf("%d\n",work);

  return 0;
}
