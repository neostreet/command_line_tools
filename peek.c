#include <stdio.h>
#include <string.h>

static char usage[] = "\
usage: peek (-title) filename (filename ...) start_line num_lines\n";
static char couldnt_open[] = "couldn't open %s\n";

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  bool bTitle;
  bool bStdin;
  int curr_arg;
  int startl;
  int num_lines;
  int endl;
  int lineno;
  int linelen;
  int titlelen;
  
  if (argc < 3) {
    printf(usage);
    return 1;
  }

  bTitle = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-title"))
      bTitle = true;
    else
      break;
  }

  if (argc - curr_arg < 2) {
    printf(usage);
    return 2;
  }

  if (argc - curr_arg == 2) {
    bStdin = true;
    fptr = stdin;
    bTitle = false;
  }
  else {
    bStdin = false;

    if (argc - curr_arg > 3)
      bTitle = true;
  }

  sscanf(argv[argc - 2],"%d",&startl);
  sscanf(argv[argc - 1],"%d",&num_lines);
  endl = startl + num_lines - 1;

  for ( ; ; ) {
    if (!bStdin) {
      if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
        printf(couldnt_open,argv[curr_arg]);

        curr_arg++;

        if (curr_arg == argc - 2)
          break;

        continue;
      }
    }

    if (bTitle) {
      if (num_lines == 1)
        printf("%s ",argv[curr_arg]);
      else {
        printf("%s\n",argv[curr_arg]);
        titlelen = strlen(argv[curr_arg]);

        for (n = 0; n < titlelen; n++)
          putchar('=');

        putchar(0x0a);
      }
    }

    lineno = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      lineno++;

      if ((lineno >= startl) && (lineno <= endl))
        printf("%s\n",line);

      if (lineno == endl)
        break;
    }

    if (bStdin)
      break;

    fclose(fptr);

    if (bTitle && num_lines != 1)
      putchar(0x0a);

    curr_arg++;

    if (curr_arg == argc - 2)
      break;
  }

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
