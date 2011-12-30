#include <stdio.h>

void main(int argc,char **argv)
{
  int n;
  int work;
  int neg;
  int not;
  int compl;

  if (argc < 2) {
    printf("usage: unary val (val ...)\n");
    return;
  }

  printf(" VALUE   COMPLMNT NEGATIVE    NOT\n");
  printf("======== ======== ======== ========\n");

  for (n = 1; n < argc; n++) {
    sscanf(argv[n],"%x",&work);
    compl = ~work;
    neg = -work;
    not = !work;
    printf("%08x %08x %08x %08x\n",work,compl,neg,not);
  }
}
