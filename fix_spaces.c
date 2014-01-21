#include <stdio.h>
#include <string.h>

#define TAB 0x09

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fix_spaces num_spaces (-prompt) infile outfile\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

char *fix_spaces(char *line,int line_len,int line_no,int num_spaces,bool bPrompt);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bPrompt;
  int m;
  int n;
  int num_spaces;
  FILE *fptr;
  FILE *ofptr;
  int line_len;
  int line_no;

  if ((argc < 4) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bPrompt = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-prompt"))
      bPrompt = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&num_spaces);

  if (!strcmp(argv[curr_arg+1],argv[curr_arg+2])) {
    printf("outfile must differ from infile\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 4;
  }

  if ((ofptr = fopen(argv[curr_arg+2],"w")) == NULL) {
    printf(couldnt_open,argv[curr_arg+2]);
    return 5;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    fprintf(ofptr,"%s\n",fix_spaces(line,line_len,line_no,num_spaces,bPrompt));
  }

  fclose(fptr);
  fclose(ofptr);

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

char *fix_spaces(char *line,int line_len,int line_no,int num_spaces,bool bPrompt)
{
  int m;
  int n;
  int tries;
  char buf[20];
  bool bFix;

  tries = line_len - num_spaces + 1;

  if (tries < 1)
    return line;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < num_spaces; n++) {
      if (line[m+n] != ' ')
        break;
    }

    if (n == num_spaces)
      break;
  }

  if (m == tries)
    return line;

  if (bPrompt) {
    printf("%3d: %s\n",line_no,line);
    printf("fix ? ",line_no,line);

    gets(buf);

    if ((buf[0] == 'y') || (buf[0] == 'Y'))
      bFix = true;
    else bFix = false;
  }
  else
    bFix = true;

  if (bFix) {
    line[m++] = TAB;

    for ( ; (line[m + num_spaces - 2]); m++)
      line[m] = line[m + num_spaces - 2];

    line[m] = 0;

    return line;
  }

  return line;
}
