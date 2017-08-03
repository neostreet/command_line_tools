#include <stdio.h>
#include <string.h>

#define MAX_STR_LEN 256

static char usage[] =
"runtot_int6 (-initial_balbal) (-verbose) (-terse) (-no_tabs) (-only_ending)\n"
"  (-only_starting) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bTerse;
  bool bNoTabs;
  bool bOnlyEnding;
  bool bOnlyStarting;
  FILE *fptr;
  int runtot;
  int work;
  char str[MAX_STR_LEN];

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  runtot = 0;
  bVerbose = false;
  bTerse = false;
  bNoTabs = false;
  bOnlyEnding = false;
  bOnlyStarting = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-initial_bal",12))
      sscanf(&argv[curr_arg][12],"%d",&runtot);
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-no_tabs"))
      bNoTabs = true;
    else if (!strcmp(argv[curr_arg],"-only_ending"))
      bOnlyEnding = true;
    else if (!strcmp(argv[curr_arg],"-only_starting"))
      bOnlyStarting = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bOnlyEnding && bOnlyStarting) {
    printf("cant' specify both -only_ending and -only_starting\n");
    return 3;
  }

  if (bVerbose && bTerse) {
    printf("can specify at most one of -verbose and -terse\n");
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 5;
  }

  for ( ; ; ) {
    fscanf(fptr,"%s\t%d",str,&work);

    if (feof(fptr))
      break;

    if (!bTerse) {
      if (bVerbose || !bNoTabs)
        printf("%s\t",str);
      else
        printf("%s ",str);
    }

    if (!bOnlyEnding) {
      if (!bVerbose) {
        if (!bNoTabs)
          printf("%d\t",runtot);
        else
          printf("%10d ",runtot);
      }
      else
        printf("%d\t%d\t",work,runtot);
    }

    runtot += work;

    if (bOnlyStarting)
      putchar(0x0a);
    else {
      if (bTerse || bVerbose || !bNoTabs)
        printf("%d\n",runtot);
      else
        printf("%10d\n",runtot);
    }
  }

  fclose(fptr);

  return 0;
}
