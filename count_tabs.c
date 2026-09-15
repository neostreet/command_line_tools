#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define TAB 0x09

static char usage[] = "usage: count_tabs (-terse) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int count_tabs(char *line,int line_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bTerse;
  FILE *fptr;
  int line_len;
  int line_no;
  int count;
  int total_count;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bTerse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else
       break;
  }

  if (bTerse)
    total_count = 0;

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    count = count_tabs(line,line_len);

    if (!bTerse)
      printf("%d\n",count);
    else
      total_count += count;
  }

  fclose(fptr);

  if (bTerse)
    printf("%d\n",total_count);

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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

static int count_tabs(char *line,int line_len)
{
  int n;
  int count;

  count = 0;

  for (n = 0; n < line_len; n++) {
    if (line[n] == TAB)
      count++;
  }

  return count;
}
