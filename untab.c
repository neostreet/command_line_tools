#include <stdio.h>

#define TAB   '\t'
#define SPACE ' '

void main(int argc,char **argv)
{
  int n;
  int num_spaces;
  FILE *fptr;
  int chara;

  if (argc != 3) {
    printf("usage: untab num_spaces filename\n");
    return;
  }

  sscanf(argv[1],"%d",&num_spaces);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf("couldn't open %s\n",argv[2]);
    return;
  }

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == TAB)
      for (n = num_spaces; (n); n--)
        putchar(SPACE);
    else
      putchar(chara);
  }

  fclose(fptr);
}
