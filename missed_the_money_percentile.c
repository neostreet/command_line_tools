#include <stdio.h>

int main(int argc,char **argv)
{
  int place;
  int entries;
  int places_paid;
  double percentile;

  if (argc != 4) {
    printf("usage: percentile place entries places_paid\n");
    return 1;
  }

  sscanf(argv[1],"%d",&place);
  sscanf(argv[2],"%d",&entries);
  sscanf(argv[3],"%d",&places_paid);

  percentile = (double)((entries - places_paid) - (place - places_paid)) / (double)(entries - places_paid);

  printf("%d %d %d %lf\n",place,entries,places_paid,percentile);

  return 0;
}
