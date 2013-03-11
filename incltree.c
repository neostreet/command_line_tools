#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "str_misc.h"
#include "str_list.h"

#define TAB 0x09

#define INCPATH_INCLUDE 0
#define LOCAL_INCLUDE   1

static char usage[] = "\
usage: incltree (-fullpath -Iinclude_dir -Iinclude_dir ...) filename\n";

static bool bFullPath;
static char **global_argv;

int num_include_dirs;

#define MAX_LINE_LEN 256
static char line[MAX_LINE_LEN];
static char full_filename[MAX_LINE_LEN];

struct str_list include_file_list;

void incltree(char *filename,int depth);
char *getincl(char *line,int *incltype);
void construct_filename(char *buf,char *filename,int pos);
void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bFullPath = false;
  num_include_dirs = 0;

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"-fullpath"))
      bFullPath = true;
    else if ((strlen(argv[n]) < 3) || (argv[n][0] != '-') || (argv[n][1] != 'I'))
      break;
    else
      num_include_dirs++;
  }

  if (n != argc - 1) {
    printf(usage);
    return 1;
  }

  global_argv = argv;

  incltree(argv[argc - 1],0);
  free_str_list(&include_file_list);

  return 0;
}

void incltree(char *filename,int depth)
{
  int m;
  int n;
  FILE *fptr;
  int linel;
  char *inclpt;
  int incltype;
  struct stat statbuf;
  int ix;

  if ((fptr = fopen(filename,"r")) == NULL) {
    printf("couldn't open %s\n",filename);
    return;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linel,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (!strncmp(line,"#include ",9)) {
      inclpt = getincl(line,&incltype);

      if (incltype != -1) {
        if (incltype == LOCAL_INCLUDE)
          n = -1;
        else
          n = 0;

        for ( ; n < num_include_dirs; n++) {
          construct_filename(full_filename,inclpt,n);

          if (!stat(full_filename,&statbuf)) {
            if (!member_of_str_list(&include_file_list,inclpt,&ix)) {
              add_str_list_elem(&include_file_list,inclpt,false);

              for (m = 0; m < depth; m++)
                printf("  ");

              if (!bFullPath)
                printf("%s\n",inclpt);
              else
                printf("%s\n",full_filename);

              incltree(full_filename,depth+1);
            }

            break;
          }
        }

        if (n == num_include_dirs) {
          fprintf(stderr,"couldn't find include file %s\n",inclpt);

          for (m = 0; m < depth; m++)
            printf("  ");

          printf("%s (not found on include path)\n",inclpt);
        }
      }
    }
  }

  fclose(fptr);
}

char *getincl(char *line,int *incltype)
{
  int n;
  int m;

  for (n = 9; (line[n]); n++) {
    if (line[n] == '<') {
      n++;
      *incltype = INCPATH_INCLUDE;

      for (m = n; (line[m]); m++) {
        if (line[m] == '>') {
          line[m] = 0;
          return &line[n];
        }
      }
    }
    else if (line[n] == '"') {
      n++;
      *incltype = LOCAL_INCLUDE;

      for (m = n; (line[m]); m++) {
        if (line[m] == '"') {
          line[m] = 0;
          return &line[n];
        }
      }
    }
  }

  *incltype = -1;
  return NULL;
}

void construct_filename(char *buf,char *filename,int pos)
{
  int n;

  if (pos == -1) {
    n = 0;
    buf[n++] = '.';
  }
  else {
    for (n = 0; (global_argv[1 + bFullPath + pos][2 + n]); n++)
      buf[n] = global_argv[1 + bFullPath + pos][2 + n];
  }

  buf[n++] = '/';
  strcpy(&buf[n],filename);
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int m;
  int n;
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

  /* remove white space from the beginning of the line */

  for (n = 0; n < local_line_len; n++) {
    if ((line[n] != ' ') && (line[n] != TAB))
      break;
  }

  if (n) {
    for (m = 0; m < local_line_len - n; m++) {
      line[m] = line[n+m];
    }

    line[m] = 0;
    local_line_len -= n;
  }

  *line_len = local_line_len;
}
