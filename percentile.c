#include <stdio.h>

int main(int argc,char **argv)
{
  int place;
  int entries;
  double percentile;

  if (argc != 3) {
    printf("usage: percentile place entries\n");
    return 1;
  }

  sscanf(argv[1],"%d",&place);
  sscanf(argv[2],"%d",&entries);

  percentile = (double)(entries - place) / (double)entries;

  printf("%d %d %lf\n",place,entries,percentile);

  return 0;
}
