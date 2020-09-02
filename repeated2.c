#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: repeated2 (-no_line_numbers) filename\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bNoLineNumbers;
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int prev_val;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bNoLineNumbers = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-no_line_numbers"))
      bNoLineNumbers = true;
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

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&val);

    if ((line_no > 1) && (val == prev_val)) {
      if (!bNoLineNumbers)
        printf("%d: %s\n",line_no,line);
      else
        printf("%s\n",line);
    }

    prev_val = val;
  }

  fclose(fptr);

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}
