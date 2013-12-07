#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

#define MAX_BUF_LEN 4096
static char buf[MAX_BUF_LEN];

static char usage[] = "usage: grabvers filename\n";
static char couldnt_open[] = "couldn't open %s\n";

#define MAX_LOG_NAME_LEN 4096
static char log_name[MAX_LOG_NAME_LEN];

#define MAX_LST_NAME_LEN 4096
static char lst_name[MAX_LST_NAME_LEN];

#define MAX_TREEISH_NAME 4096
static char treeish_name[MAX_TREEISH_NAME];

static char commit[] = "commit ";
#define COMMIT_LEN (sizeof (commit) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static void fixup_filename(char *filename,int len);

int main(int argc,char **argv)
{
  FILE *fptr;
  FILE *lst_fptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  sprintf(log_name,"%s.log",argv[1]);
  fixup_filename(log_name,strlen(log_name));

  sprintf(lst_name,"%s.lst",argv[1]);
  fixup_filename(lst_name,strlen(lst_name));

  sprintf(buf,"git log %s > %s",argv[1],log_name);
  system(buf);

  if ((fptr = fopen(log_name,"r")) == NULL) {
    printf(couldnt_open,log_name);
    return 2;
  }

  if ((lst_fptr = fopen(lst_name,"w")) == NULL) {
    printf(couldnt_open,log_name);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (!strncmp(line,commit,COMMIT_LEN)) {
      sprintf(treeish_name,"%s:%s",&line[COMMIT_LEN],argv[1]);
      sprintf(buf,"git show %s > ",treeish_name,treeish_name);
      fixup_filename(treeish_name,strlen(treeish_name));
      fprintf(lst_fptr,"%s\n",treeish_name);
      strcat(buf,treeish_name);
      system(buf);
    }
  }

  fclose(lst_fptr);
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

static void fixup_filename(char *filename,int len)
{
  int n;

  for (n = 0; n < len; n++)
    if ((filename[n] == '/') || (filename[n] == ':'))
      filename[n] = '.';
}
