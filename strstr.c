#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
  char *cpt;

  if (argc != 3) {
    printf("usage: strstr haystack needle\n");
    return 1;
  }

  cpt = strstr(argv[1],argv[2]);

  if (cpt != NULL)
    printf("%s\n",cpt);

  return 0;
}
