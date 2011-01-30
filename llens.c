#include <stdio.h>

FILE *fptr;

int main(int argc,char **argv)
{
  int n;
  int chara;
  int cur_lln;

  if (argc < 2) {
    printf("usage: llens filename (filename ...)\n");
    return 1;
  }

  for (n = 1; n < argc; n++) {
    if ((fptr = fopen(argv[1],"r")) == NULL) {
      printf("couldn't open %s\n",argv[1]);
      continue;
    }

    cur_lln = 0;

    for ( ; ; ) {
      chara = fgetc(fptr);

      if (feof(fptr))
        break;

      if (chara == 0x0a) {
        printf("%d\n",cur_lln);
        cur_lln = 0;
      }
      else
        cur_lln++;
    }

    fclose(fptr);
  }

  return 0;
}
