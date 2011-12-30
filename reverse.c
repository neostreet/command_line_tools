#include <stdio.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <string.h>

void main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  size_t mem_amount;
  char *mempt;
  FILE *fptr;
  char chara;

  if (argc != 2) {
    printf("usage: reverse filename\n");
    return;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf("couldn't get status of %s\n",argv[1]);
    return;
  }
  else
    mem_amount = (size_t)statbuf.st_size;

  if ((mempt = malloc(mem_amount)) == NULL) {
    printf("malloc call failed\n");
    return;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    free(mempt);
    return;
  }

  for (n = 0; ; n++) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    mempt[n] = chara;
  }

  n--;

  for ( ; ((n >= 0) && (mempt[n] != '\n')); n--)
    ;

  if (n < 0)
    return;

  mempt[n] = 0;
  n--;

  for ( ; ; ) {
    for ( ; ((n >= 0) && (mempt[n] != '\n')); n--)
      ;

    n++;
    printf("%s\n",&mempt[n]);

    if (!n)
      break;
    else {
      n--;
      mempt[n] = 0;
    }
  }

  free(mempt);
}
