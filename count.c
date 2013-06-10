#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 2048
char line[MAX_LINE_LEN];

static char usage[] =
"usage: count (-c) (-by_line) string filename (filename ...)\n";
static char fmt_str[] = "%s: %d\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int chara;
  int offset;
  int linelen;
  int searchlen;
  int line_no;
  int tries;
  int m;
  int n;
  bool bCaseSens;
  bool bByLine;
  bool bStdin;
  int curr_arg;
  int count;
  int tcount;
  int line_count;
  int string_arg;
  bool bMultiple;

  bCaseSens = false;
  bByLine = false;
  bStdin = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-c"))
      bCaseSens = true;
    else if (!strcmp(argv[curr_arg],"-by_line"))
      bByLine = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 1;
  }

  string_arg = curr_arg;

  for (offset = 0; (chara = argv[curr_arg][offset]); offset++) {
    if (!bCaseSens) {
      if ((chara >= 'A') && (chara <= 'Z'))
        argv[curr_arg][offset] = (char)(chara - 'A' + 'a');
    }
  }

  searchlen = offset;

  if (argc - curr_arg == 1)
    bStdin = true;
  else {
    curr_arg++;

    bMultiple = (argc - curr_arg > 1);
  }

  tcount = 0;

  for ( ; ; ) {

  if (!bStdin) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf("couldn't open %s\n",argv[curr_arg]);

      curr_arg++;

      if (curr_arg == argc)
        break;

      continue;
    }
  }
  else
    fptr = stdin;

  count = 0;

  for (line_no = 1; ; line_no++) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr)) {
      if (!bByLine && count) {
        if (!bStdin) {
          if (bMultiple)
            printf(fmt_str,argv[curr_arg],count);
          else
            printf(fmt_str,argv[string_arg],count);
        }
        else
          printf("%d\n",count);

        tcount += count;
      }

      if (!bStdin)
        fclose(fptr);

      break;
    }

    if (linelen >= searchlen) {
      if (bByLine)
        line_count = 0;

      tries = linelen - searchlen + 1;

      for (m = 0; m < tries; m++) {
        for (n = 0; n < searchlen; n++) {
          chara = line[m+n];

          if (!bCaseSens)
            if ((chara >= 'A') && (chara <= 'Z'))
              chara -= 'A' - 'a';

          if (chara != argv[string_arg][n])
            break;
        }

        if (n == searchlen) {
          if (!bByLine)
            count++;
          else
            line_count++;
        }
      }

      if (bByLine && line_count)
        printf("%5d: %5d\n",line_no,line_count);
    }
  }

  if (bStdin)
    break;
  else {
    curr_arg++;

    if (curr_arg == argc)
      break;
  }

  }

  if ((argc - bCaseSens > 3) && (tcount))
    printf("\n%4d\n",tcount);

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
