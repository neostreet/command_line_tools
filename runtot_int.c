#include <stdio.h>
#include <string.h>

static char usage[] = "runtot (-initial_balbal) (-verbose) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  int runtot;
  int work;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12))
      sscanf(&argv[curr_arg][12],"%d",&runtot);
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    runtot += work;

    if (!bVerbose)
      printf("%d\n",runtot);
    else
      printf("%d %d\n",work,runtot);
  }

  fclose(fptr);

  return 0;
}
