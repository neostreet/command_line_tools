#include <stdio.h>

FILE *fptr;

int main(int argc,char **argv)
{
  int curr_arg;
  int chara;
  int cur_l;
  int cur_lln;
  int min_l;
  int min_lln;
  int lines_of_min_len;
  int tot_lines;

  if (argc < 2) {
    printf("usage: minllen filename (filename ...)\n");
    return 1;
  }

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf("Couldn't open %s\n",argv[curr_arg]);
      continue;
    }

    cur_l = 1;

    min_lln = 1024;
    cur_lln = 0;
    lines_of_min_len = 0;
    tot_lines = 0;

    for ( ; ; ) {
      chara = fgetc(fptr);

      if (feof(fptr))
        break;

      if (chara == 0x0a) {
        if (cur_lln < min_lln) {
          min_lln = cur_lln;
          min_l = cur_l;
          lines_of_min_len = 1;
        }
        else if (cur_lln == min_lln)
          lines_of_min_len++;

        tot_lines++;

        cur_lln = 0;
        cur_l++;
      }
      else
        cur_lln++;
    }

    fclose(fptr);
    printf("%s: %d (%d, %d of %d total lines)\n",
      argv[curr_arg],min_lln,min_l,lines_of_min_len,tot_lines);
  }

  return 0;
}
