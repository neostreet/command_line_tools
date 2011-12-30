#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bSuffix;
  char *suffix;
  int start_val;
  int end_val;

  if ((argc < 4) || (argc > 5)) {
    printf("usage: gen_list (-suffixsuffix) prefix start_val end_val\n");
    return 1;
  }

  bSuffix = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-suffix",7)) {
      suffix = &argv[curr_arg][7];
      bSuffix = TRUE;
    }
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf("usage: gen_list (-suffixsuffix) prefix start_val end_val\n");
    return 2;
  }

  sscanf(argv[curr_arg+1],"%d",&start_val);
  sscanf(argv[curr_arg+2],"%d",&end_val);

  if (start_val <= end_val) {
    for (n = start_val; n <= end_val; n++) {
      if (!bSuffix)
        printf("%s%d\n",argv[curr_arg],n);
      else
        printf("%s%d%s\n",argv[curr_arg],n,suffix);
    }
  }
  else {
    for (n = start_val; n >= end_val; n--) {
      if (!bSuffix)
        printf("%s%d\n",argv[curr_arg],n);
      else
        printf("%s%d%s\n",argv[curr_arg],n,suffix);
    }
  }

  return 0;
}
