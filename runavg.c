#include <stdio.h>

int main(int argc,char **argv)
{
  FILE *fptr;
  int nobs;
  double runtot;
  double work;
  double avg;

  if (argc < 2) {
    printf("usage: runavg filename\n");
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return 2;
  }

  nobs = 0;
  runtot = (double)0;

  for ( ; ; ) {
    fscanf(fptr,"%lf",&work);

    if (feof(fptr))
      break;

    nobs++;

    runtot += work;

    avg = runtot / (double)nobs;

    printf("%10.2lf %10.2lf\n",work,avg);
  }

  fclose(fptr);

  return 0;
}
