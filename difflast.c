#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

char temp_filename[] = "difflast.tmp";

#define MAX_LINE_LEN 1024
static char line1[MAX_LINE_LEN];
static char line2[MAX_LINE_LEN];

#define MAX_BUF_LEN 1024
static char buf[MAX_BUF_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

static char couldnt_open[] = "couldn't open %s\n";
static char unexpected_eof[] = "unexpected eof in %s\n";

int main(int argc,char **argv)
{
  FILE *fptr;
  int linelen;

  if (argc != 2) {
    printf("usage: difflast filename\n");
    return 1;
  }

  sprintf(buf,"ls -1t %s* > %s",argv[1],temp_filename);
  system(buf);

  if ((fptr = fopen(temp_filename,"r")) == NULL) {
    printf(couldnt_open,temp_filename);
    return 2;
  }

  GetLine(fptr,line1,&linelen,MAX_LINE_LEN);

  if (feof(fptr)) {
    printf(unexpected_eof,temp_filename);
    return 3;
  }

  GetLine(fptr,line2,&linelen,MAX_LINE_LEN);

  if (feof(fptr)) {
    printf(unexpected_eof,temp_filename);
    return 4;
  }

  fclose(fptr);
  remove(temp_filename);

  sprintf(buf,"kdiff3 %s %s",line2,line1);
  system(buf);

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

char temp_filename[] = "difflast.tmp";

#define MAX_LINE_LEN 1024
static char line1[MAX_LINE_LEN];
static char line2[MAX_LINE_LEN];

#define MAX_BUF_LEN 1024
static char buf[MAX_BUF_LEN];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

static char couldnt_open[] = "couldn't open %s\n";
static char unexpected_eof[] = "unexpected eof in %s\n";

int main(int argc,char **argv)
{
  FILE *fptr;
  int linelen;

  if (argc != 2) {
    printf("usage: difflast filename\n");
    return 1;
  }

  sprintf(buf,"dir /b /o-d %s* > %s",argv[1],temp_filename);
  system(buf);

  if ((fptr = fopen(temp_filename,"r")) == NULL) {
    printf(couldnt_open,temp_filename);
    return 2;
  }

  GetLine(fptr,line1,&linelen,MAX_LINE_LEN);

  if (feof(fptr)) {
    printf(unexpected_eof,temp_filename);
    return 3;
  }

  GetLine(fptr,line2,&linelen,MAX_LINE_LEN);

  if (feof(fptr)) {
    printf(unexpected_eof,temp_filename);
    return 4;
  }

  fclose(fptr);
  remove(temp_filename);

  sprintf(buf,"windiff %s %s",line2,line1);
  system(buf);

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
