#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define MAX_EXTS 1024

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#ifndef WIN32
#define strcmpi strcmp
#define strnicmp strncmp
#endif

#define MAX_EXT_LEN 80

struct ext_info {
  char ext[MAX_EXT_LEN+1];
  int  ext_count;
};

static char usage[] =
"usage: exts (-sort_count_desc) (-delimdelim) (filename ...)\n";
static struct ext_info exts[MAX_EXTS];
static int num_exts;
static int ext_ixs[MAX_EXTS];

int get_ext(char *string,int delimiter,char *ext);
int elem_compare(const void *elem1,const void *elem2);
int elem_compare2(const void *elem1,const void *elem2);
int contains(char chara,char *str);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bSortCountDesc;
  int bNonDefaultDelim;
  int delimiter;
  int bStdin;
  char *filename;
  int linelen;
  char cur_ext[MAX_EXT_LEN+1];
  int tot_files;
  int tot_files_with_ext;

  bSortCountDesc = FALSE;
  bNonDefaultDelim = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmpi(argv[curr_arg],"-sort_count_desc"))
      bSortCountDesc = TRUE;
    else if (!strnicmp(argv[curr_arg],"-delim",6)) {
      bNonDefaultDelim = TRUE;
      delimiter = argv[curr_arg][6];
    }
    else
      break;
  }

  if (!bNonDefaultDelim) {
#ifdef WIN32
    delimiter = '\\';
#else
    delimiter = '/';
#endif
  }

  if (curr_arg == argc) {
    bStdin = TRUE;
    filename = line;
  }
  else
    bStdin = FALSE;

  tot_files = 0;
  tot_files_with_ext = 0;
  num_exts = 0;

  for ( ; ; ) {
    if (bStdin) {
      GetLine(stdin,line,&linelen,MAX_LINE_LEN);

      if (feof(stdin))
        break;
    }
    else
      filename = argv[curr_arg];

    tot_files++;

    if (contains('*',filename) ||
      !get_ext(filename,delimiter,cur_ext))
      printf("%s\n",filename);
    else {
      tot_files_with_ext++;

      for (n = 0; n < num_exts; n++) {
        if (!strcmpi(cur_ext,exts[n].ext)) {
          exts[n].ext_count++;
          break;
        }
      }

      if (n == num_exts) {
        if (num_exts < MAX_EXTS) {
          strcpy(exts[num_exts].ext,cur_ext);
          exts[num_exts++].ext_count = 1;
        }
      }
    }

    if (!bStdin) {
      curr_arg++;

      if (curr_arg == argc)
        break;
    }
  }

  for (n = 0; n < num_exts; n++)
    ext_ixs[n] = n;

  if (!bSortCountDesc)
    qsort(ext_ixs,num_exts,sizeof (int),elem_compare);
  else
    qsort(ext_ixs,num_exts,sizeof (int),elem_compare2);

  for (n = 0; n < num_exts; n++) {
    printf("%3d %s\n",
      exts[ext_ixs[n]].ext_count,
      exts[ext_ixs[n]].ext);
  }

  printf("---\n");
  printf("%3d (%d + %d)\n",tot_files,
    tot_files - tot_files_with_ext,tot_files_with_ext);

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

int get_ext(char *string,int delimiter,char *ext)
{
  int m;
  int n;
  int len;

  len = strlen(string);

  for (n = len - 1; (n >= 0); n--) {
    if (string[n] == delimiter)
      break;
  }

  m = n + 1;

  for (n = len - 1; (n >= m); n--) {
    if (string[n] == '.')
      break;
  }

  if (n > m) {
    strncpy(ext,&string[n+1],MAX_EXT_LEN);
    return TRUE;
  }

  return FALSE;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int *ipt1;
  int *ipt2;
  char *cpt1;
  char *cpt2;

  ipt1 = (int *)elem1;
  ipt2 = (int *)elem2;

  cpt1 = exts[*ipt1].ext;
  cpt2 = exts[*ipt2].ext;

  return strcmp(cpt1,cpt2);
}

int elem_compare2(const void *elem1,const void *elem2)
{
  int *ipt1;
  int *ipt2;

  ipt1 = (int *)elem1;
  ipt2 = (int *)elem2;

  return exts[*ipt2].ext_count - exts[*ipt1].ext_count;
}

int contains(char chara,char *str)
{
  int n;

  for (n = 0; (str[n]); n++)
    if (str[n] == chara)
      return 1;

  return 0;
}
