#include <stdio.h>
#include <string.h>

#define MAX_STR_LEN 256

static char usage[] =
"usage: runtot_int3 (-initial_balbal) (-verbose) (-terse) (-star_blue)\n"
"  (-no_runtot) (-balance_first) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bTerse;
  bool bStarBlue;
  bool bNoRuntot;
  bool bBalanceFirst;
  FILE *fptr;
  int runtot;
  int max;
  int blue;
  int work;
  char str[MAX_STR_LEN];

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  max = 0;
  bVerbose = false;
  bTerse = false;
  bStarBlue = false;
  bNoRuntot = false;
  bBalanceFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12)) {
      sscanf(&argv[curr_arg][12],"%d",&runtot);
      max = runtot;
    }
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-star_blue"))
      bStarBlue = true;
    else if (!strcmp(argv[curr_arg],"-no_runtot"))
      bNoRuntot = true;
    else if (!strcmp(argv[curr_arg],"-balance_first"))
      bBalanceFirst = true;
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
    fscanf(fptr,"%s\t%d",str,&work);

    if (feof(fptr))
      break;

    runtot += work;

    if (runtot > max) {
      blue = 1;
      max = runtot;
    }
    else
      blue = 0;

    if (!bVerbose) {
      if (!bTerse) {
        if (!bStarBlue || !blue) {
          if (!bNoRuntot) {
            if (!bBalanceFirst)
              printf("%s\t%d\n",str,runtot);
            else
              printf("%d\t%s\n",runtot,str);
          }
          else
            printf("%s\n",str);
        }
        else if (!bNoRuntot)
          printf("%s\t%d *\n",str,runtot);
        else
          printf("%s\t*\n",str);
      }
      else {
        if (!bStarBlue || !blue)
          printf("%d\n",runtot);
        else
          printf("%d *\n",runtot);
      }
    }
    else {
      if (!bStarBlue || !blue)
        printf("%s\t%d %d\n",str,work,runtot);
      else
        printf("%s\t%d %d *\n",str,work,runtot);
    }
  }

  fclose(fptr);

  return 0;
}
