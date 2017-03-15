#include <stdio.h>

int main(int argc,char **argv)
{
  int place;
  int entries;
  double percentile;

  if (argc != 3) {
    printf("usage: percentile entries place\n");
    return 1;
  }

  sscanf(argv[1],"%d",&entries);
  sscanf(argv[2],"%d",&place);

  percentile = (double)(entries - place) / (double)entries;

  printf("%lf %d %d\n",percentile,entries,place);

  return 0;
}
