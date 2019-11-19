#include <stdio.h>
#include <string.h>

static char usage[] = "usage: zorch (-leading_only) infile\n";

#define MAXLLEN 512
static char line[MAXLLEN];

void get_zorched_line(FILE *fptr,char *line,int maxllen,int *llenpt,int leading_only);

int main(int argc,char **argv)
{
  FILE *fptr;
  int linelen;
  int leading_only;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"-leading_only"))
    leading_only = 1;
  else
    leading_only = 0;

  if (argc - leading_only != 2) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[argc - 1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[argc - 1]);
    return 3;
  }

  for ( ; ; ) {
    get_zorched_line(fptr,line,MAXLLEN,&linelen,leading_only);

    if (feof(fptr))
      break;

    if (linelen)
      printf("%s\n",line);
  }

  fclose(fptr);

  return 0;
}

void get_zorched_line(FILE *fptr,char *line,int maxllen,int *llenpt,int leading_only)
{
  int chara;
  int linelen;

  linelen = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == 0x0a)
      break;

    if (linelen < maxllen - 1) {
      if ((chara != ' ') && (chara != 0x09))
        line[linelen++] = chara;
      else if (leading_only && linelen)
        line[linelen++] = chara;
    }
  }

  line[linelen] = 0;
  *llenpt = linelen;
}
