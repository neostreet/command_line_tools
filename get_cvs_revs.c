#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char tempfile1_name[] = "get_cvs_revs.tmp1";
static char tempfile2_name[] = "get_cvs_revs.tmp2";
#define MAX_TEMPFILE_FULLPATH_LEN 2048
static char tempfile1_fullpath[MAX_TEMPFILE_FULLPATH_LEN+1];
static char tempfile2_fullpath[MAX_TEMPFILE_FULLPATH_LEN+1];

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

#define MAX_COMMAND_LEN 4096
static char command[MAX_COMMAND_LEN];

static char usage[] = "usage: get_cvs_revs cvsfilename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void add_revision_to_rename_command(char *revision,char *command);

int main(int argc,char **argv)
{
  int n;
  int len;
  int barefilename_ix;
  char *tempdir;
  char *cvstempdir;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  len = strlen(argv[1]);

  for (n = len - 1; (n >= 0); n--) {
    if (argv[1][n] == '/')
      break;
  }

  if (n < 0) {
    printf("cvsfilename must contain at least one forward slash\n");
    return 2;
  }

  barefilename_ix = n + 1;

  tempdir = getenv("TEMP");
  sprintf(tempfile1_fullpath,"%s\\%s",tempdir,tempfile1_name);
  sprintf(tempfile2_fullpath,"%s\\%s",tempdir,tempfile2_name);

  cvstempdir = getenv("CVSTEMP");

  sprintf(command,"cvs log %s > %s",&argv[1][barefilename_ix],tempfile1_fullpath);

  system(command);

  if ((fptr = fopen(tempfile1_fullpath,"r")) == NULL) {
    printf(couldnt_open,tempfile1_fullpath);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (!strncmp(line,"revision ",9)) {
      sprintf(command,"cvs -q checkout -r %s -d %s \"%s\" > %s",
        &line[9],cvstempdir,argv[1],tempfile2_fullpath);

      system(command);

      sprintf(command,"rename %s\\%s %s",
        cvstempdir,
        &argv[1][barefilename_ix],
        &argv[1][barefilename_ix]);

      add_revision_to_rename_command(&line[9],command);

      system(command);
    }
  }

  fclose(fptr);

  remove(tempfile1_fullpath);
  remove(tempfile2_fullpath);

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

void add_revision_to_rename_command(char *revision,char *command)
{
  int m;
  int p;
  int revision_len;
  int command_len;
  int final_dot_ix;
  int suffix_len;

  revision_len = strlen(revision);
  command_len = strlen(command);

  for (final_dot_ix = command_len - 1; (final_dot_ix >= 0); final_dot_ix--) {
    if (command[final_dot_ix] == '.')
      break;
  }

  if (final_dot_ix < 0) {
    sprintf(&command[command_len],".%s",revision);
    return;
  }

  suffix_len = command_len - final_dot_ix;

  command[command_len + revision_len + 1] = 0;

  for (m = 0; m < suffix_len; m++) {
    p = command_len + revision_len - m;
    command[p] = command[p - revision_len - 1];
  }

  for (m = 0; m < revision_len; m++)
    command[final_dot_ix + m + 1] = revision[m];
}
