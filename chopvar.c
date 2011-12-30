#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void chop(char *cpt);

#ifdef WNT
static char delimiter = ';';
#else
static char delimiter = ':';
#endif

int main(int argc,char **argv)
{
  char *cpt;

  if (argc != 2) {
    printf("usage: chopvar varname\n");
    return 1;
  }

  cpt = getenv(argv[1]);

  if (cpt == NULL) {
    printf("environment variable %s not found\n",argv[1]);
    return 2;
  }

  chop(cpt);

  return 0;
}

void chop(char *cpt)
{
  int m;
  int n;
  int len;

  len = strlen(cpt);
  n = 0;

  for ( ; ; ) {
    m = n;

    for ( ; ; ) {
      if (cpt[n] == delimiter)
        break;

      n++;

      if (n == len)
        break;
    }

    if (cpt[n] == delimiter)
      cpt[n] = 0;

    printf("%s\n",&cpt[m]);

    if (n == len)
      break;

    n++;

    if (n == len)
      break;
  }
}
