#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "usage: tandem_grep2 str1 str2 filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int str1_len;
  int str2_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  str1_len = strlen(argv[1]);
  str2_len = strlen(argv[2]);

  if ((fptr = fopen(argv[3],"r")) == NULL) {
    printf(couldnt_open,argv[3]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (Contains(true,
      line,line_len,
      argv[1],str1_len,
      &ix)) {

      printf("%s\n",line);
    }
    else if (Contains(true,
      line,line_len,
      argv[2],str2_len,
      &ix)) {

      printf("%s\n",line);
    }
  }

  fclose(fptr);

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

static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    return false;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < string_len; n++) {
      chara = line[m + n];

      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          chara += 'a' - 'A';
      }

      if (chara != string[n])
        break;
    }

    if (n == string_len) {
      *index = m;
      return true;
    }
  }

  return false;
}
