#include <stdio.h>
#include <string.h>

#define DOT '.'
#define SPACE ' '
#define CAPTURE 'x'
#define CHECK '+'
#define MATE '#'

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

#define MAX_FILENAME_LEN 256

static char usage[] = "\
usage: cgfilt (-qnn) [white | black] filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static int word_count;

static char txt_ext[] = "txt";
#define TXT_EXT_LEN (sizeof txt_ext - 1)

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void filter(FILE *fptr,char *line,int line_len);
int fprint_word(FILE *fptr,char *word,int word_len);
int create_outname(char *filename,char *outname,int max_outname_len);

int main(int argc,char **argv)
{
  int curr_arg;
  int quiz_number;
  int initial_move;
  bool bBlack;
  FILE *fptr[2];
  int linelen;
  int retval;
  char outname[MAX_FILENAME_LEN];

  if ((argc != 3) && (argc != 4)) {
    printf(usage);
    return 1;
  }

  quiz_number = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-qn",3))
      sscanf(&argv[curr_arg][3],"%d",&quiz_number);
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg],"white"))
    bBlack = false;
  else if (!strcmp(argv[curr_arg],"black"))
    bBlack = true;
  else {
    printf(usage);
    return 3;
  }

  curr_arg++;

  if ((fptr[0] = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  retval = create_outname(argv[curr_arg],outname,MAX_FILENAME_LEN);

  if (retval) {
    printf("create_outname() of file %s failed: %d\n",argv[curr_arg],retval);
    fclose(fptr[0]);
    return 5;
  }

  if ((fptr[1] = fopen(outname,"w")) == NULL) {
    printf(couldnt_open,outname);
    fclose(fptr[0]);
    return 6;
  }

  fprintf(fptr[1],"%d\n\n",bBlack);

  if (quiz_number != -1) {
    initial_move = (quiz_number - 1) * 2;

    if (bBlack)
      initial_move++;

    fprintf(fptr[1],"initial_move%d\n",initial_move);
    fprintf(fptr[1],"/ quiz %2d. ",quiz_number);

    if (bBlack)
      fprintf(fptr[1],"... ?\n\n");
    else
      fprintf(fptr[1],"?\n\n");
  }

  for ( ; ; ) {
    GetLine(fptr[0],line,&linelen,MAX_LINE_LEN);

    if (feof(fptr[0]))
      break;

    if (linelen && (line[0] != '['))
      filter(fptr[1],line,linelen);
  }

  if (word_count == 1)
    fputc(0x0a,fptr[1]);

  fclose(fptr[0]);
  fclose(fptr[1]);

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

void filter(FILE *fptr,char *line,int line_len)
{
  int ix;
  int sav_ix;

  ix = 0;

  for ( ; ; ) {
    /* get next word */
    sav_ix = ix;

    for ( ; (ix < line_len) && (line[ix] != SPACE); ix++)
      ;

    if (sav_ix == ix)
      break;

    line[ix] = 0;
    word_count += fprint_word(fptr,&line[sav_ix],ix - sav_ix);

    if (word_count == 2) {
      fputc(0x0a,fptr);
      word_count = 0;
    }

    if (ix == line_len)
      break;

    ix++;
  }
}

int fprint_word(FILE *fptr,char *word,int word_len)
{
  int ix;

  if (word[word_len - 1] == DOT) {
    fprintf(fptr,"%s ",word);
    return 0;
  }

  if (!strcmp(word,"1-0"))
    return 0;

  if (!strcmp(word,"0-1"))
    return 0;

  for (ix = 0; ix < word_len; ix++) {
    if ((word[ix] != CAPTURE) && (word[ix] != CHECK) && (word[ix] != MATE))
      fputc(word[ix],fptr);

    if (word[ix] == DOT)
      fputc(SPACE,fptr);
  }

  fputc(SPACE,fptr);

  return 1;
}

int create_outname(char *filename,char *outname,int max_outname_len)
{
  int m;
  int n;

  for (n = strlen(filename) - 1; (n >= 0); n--) {
    if (filename[n] == '.')
      break;
  }

  if (n < 0)
    return 1;

  n++;

  if (n + (int)TXT_EXT_LEN + 1 > max_outname_len)
    return 2;

  for (m = 0; m < n; m++)
    outname[m] = filename[m];

  for (n = 0; n < TXT_EXT_LEN; n++)
    outname[m+n] = txt_ext[n];

  outname[m+TXT_EXT_LEN] = 0;

  return 0;
}
