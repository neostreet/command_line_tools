#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
  if (argc != 2) {
    printf("usage: getenv var\n");
    return 1;
  }

  printf("%s\n",getenv(argv[1]));

  return 0;
}
