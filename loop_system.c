#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc,char **argv)
{
  int n;
  int loop_count;
  time_t start_time;
  time_t end_time;

  if (argc != 3) {
    printf("usage: loop_system loop_count command\n");
    return 1;
  }

  sscanf(argv[1],"%d",&loop_count);

  time(&start_time);

  for (n = 0; n < loop_count; n++)
    system(argv[2]);

  time(&end_time);

  printf("\nprocessing time: %d seconds\n",end_time - start_time);

  return 0;
}
