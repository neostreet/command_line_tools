#include <stdio.h>
#include <string.h>

int main(int argc,char **argv)
{
  int n;

  if ((argc > 1) && !strcmp(argv[1],"-detail")) {
    for (n = 2; n < argc; n++) {
      printf("%s\n",argv[n]);

      if (n == argc - 1)
        putchar(0x0a);
    }

    printf("%d\n",argc - 2);
  }
  else
    printf("%d\n",argc - 1);

  return 0;
}
