#include <stdio.h>
#include <stdlib.h>

void main(int argc,char **argv)
{
  FILE *fptr;

  if (argc != 3) {
    printf("usage: rse filename \"command\"\n");
    return;
  }

  if ((fptr = freopen(argv[1],"w",stderr)) == NULL) {
    printf("couldn't freopen stderr to %s\n",argv[1]);
    return;
  }

  system(argv[2]);
}
