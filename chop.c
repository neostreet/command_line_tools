#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAB 0x09
#define BACKSLASH 0x5c

#define MAX_LINE_LEN 32768
char *line;
char *buf;

static char usage[] =
"usage: chop (-drop_delim) (-no_trim) delimiter filename\n";
static char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

void trim_line(char *line,int *linelen,char delimiter);
void chop(char *cpt,char delimiter,bool bDropDelim);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDropDelim;
  bool bNoTrim;
  char delimiter;
  FILE *fptr;
  int linelen;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  if ((line = (char *)malloc(MAX_LINE_LEN)) == NULL) {
    printf("couldn't allocate memory for line\n");
    return 2;
  }

  if ((buf = (char *)malloc(MAX_LINE_LEN)) == NULL) {
    printf("couldn't allocate memory for buf\n");
    free(line);
    return 3;
  }

  bDropDelim = false;
  bNoTrim = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-drop_delim"))
      bDropDelim = true;
    else if (!strcmp(argv[curr_arg],"-no_trim"))
      bNoTrim = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    free(buf);
    free(line);
    return 4;
  }

  if (!strcmp(argv[curr_arg],"TAB"))
    delimiter = TAB;
  else if (!strcmp(argv[curr_arg],"BACKSLASH"))
    delimiter = BACKSLASH;
  else
    delimiter = argv[curr_arg][0];

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    free(buf);
    free(line);
    return 5;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (bDropDelim && !bNoTrim)
      trim_line(line,&linelen,delimiter);

    chop(line,delimiter,bDropDelim);
  }

  fclose(fptr);

  free(buf);
  free(line);

  return 0;
}

void chop(char *cpt,char delimiter,bool bDropDelim)
{
  int m;
  int n;
  int p;
  int len;

  len = strlen(cpt);

  if (!len) {
    printf("\n");
    return;
  }

  n = 0;

  for ( ; ; ) {
    m = n;

    for ( ; ; ) {
      if (cpt[n] == delimiter)
        break;

      n++;

      if (n == len)
        break;
    }

    for (p = 0; p <= n - (m + bDropDelim); p++)
      buf[p] = cpt[m+p];

    buf[p] = 0;
    printf("%s\n",buf);

    if (n == len)
      break;

    n++;

    if (n == len)
      break;
  }
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

void trim_line(char *line,int *linelen,char delimiter)
{
  int m;
  int n;
  int linel;

  m = 0;
  linel = *linelen;

  /* skip to the first non-delimiter character in the line */

  for (n = 0; n < linel; n++) {
    if (line[n] != delimiter)
      break;
  }

  if (n == linel) {
    line[m] = 0;
    return;
  }

  for ( ; n < linel; n++) {
    if (line[n] != delimiter) {
      if (m != n)
        line[m] = line[n];

      m++;
    }
    else if (!n || (line[n-1] != delimiter)) {
      if (m != n)
        line[m] = line[n];

      m++;
    }
  }

  line[m] = 0;
  *linelen = m;
}
