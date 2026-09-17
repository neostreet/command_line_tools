#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: count_chara (-terse) (-not) chara filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int count_chara(char chara,char *line,int line_len,bool bNot);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bTerse;
  bool bNot;
  char chara;
  FILE *fptr;
  int line_len;
  int line_no;
  int count;
  int total_count;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bNot = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else
       break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 2;
  }

  if (bTerse)
    total_count = 0;

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    count = count_chara(argv[curr_arg][0],line,line_len,bNot);

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

static int count_chara(char chara,char *line,int line_len,bool bNot)
{
  int n;
  int count;

  count = 0;

  for (n = 0; n < line_len; n++) {
    if (!bNot) {
      if (line[n] == chara)
        count++;
    }
    else {
      if (line[n] != chara)
        count++;
    }
  }

  return count;
}
