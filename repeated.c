#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];
static char prev_line[MAX_LINE_LEN];

static char usage[] = "usage: repeated offset len filename\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int offset;
  int len;
  FILE *fptr;
  int linel;
  int lineno;
  int repeat_count;

  if (argc != 4) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&offset);
  sscanf(argv[2],"%d",&len);

  if ((fptr = fopen(argv[3],"r")) == NULL) {
    printf("couldn't open %s\n",argv[3]);
    return 2;
  }

  lineno = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linel,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    lineno++;

    if ((lineno > 1) && !strncmp(&prev_line[offset],&line[offset],len)) {
      repeat_count++;
      printf("%7d (%d): %s\n",lineno,repeat_count,line);
    }
    else
      repeat_count = 0;

    strcpy(prev_line,line);
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
