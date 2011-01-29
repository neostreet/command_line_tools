#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE  1

#define NEWLINE 0x0a

#define MAX_LINE_LEN 1024
#define MAX_FILENAME_LEN MAX_LINE_LEN
char line[MAX_LINE_LEN];

static char couldnt_open[] = "couldn't open %s\n";

struct file_info {
  char name[MAX_FILENAME_LEN];
  int lines;
};

static struct file_info *finfo;

int compare(const void *elem1,const void *elem2);

#ifndef WIN32
#define strcmpi strcmp
#endif

static char fmt_str[] = "%6d %s\n";
static char total_str[] = "TOTAL";

int main(int argc,char **argv)
{
  int m;
  int n;
  int num_files;
  int *ixs;
  FILE *fptr;
  long tot_lines;
  long lines;
  int chara;
  int svchara;
  int curr_arg;
  int bTotalOnly;
  int bStdinIsList;
  int bStdin;
  int bNoSort;

  bTotalOnly = FALSE;
  bStdinIsList = FALSE;
  bNoSort = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmpi(argv[curr_arg],"-totalonly"))
      bTotalOnly = TRUE;
    else if (!strcmpi(argv[curr_arg],"-stdinislist"))
      bStdinIsList = TRUE;
    else if (!strcmpi(argv[curr_arg],"-no_sort"))
      bNoSort = TRUE;
    else
      break;
  }

  if (curr_arg == argc)
    bStdin = 1;
  else {
    bStdin = 0;

    num_files = argc - curr_arg;

    finfo = (struct file_info *)malloc(num_files * sizeof (struct file_info));

    if (finfo == NULL) {
      printf("couldn't malloc %d bytes\n",
        num_files * sizeof (struct file_info));
      return 1;
    }

    ixs = (int *)malloc(num_files * sizeof (int));

    if (ixs == NULL) {
      printf("couldn't malloc %d bytes\n",num_files * sizeof (int));
      return 2;
    }
  }

  tot_lines = 0;
  m = 0;

  /* start of argument loop: */
  for ( ; ; ) {

  if (bStdin)
    fptr = stdin;
  else if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    curr_arg++;

    if (curr_arg == argc)
      break;

    continue;
  }

  lines = 0;
  svchara = NEWLINE;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    svchara = chara;

    if (svchara == NEWLINE)
      lines++;
  }

  fclose(fptr);

  if (svchara != NEWLINE)
    lines++;

  if (bStdin)
    break;

  strcpy(finfo[m].name,argv[curr_arg]);
  finfo[m].lines = lines;
  m++;

  tot_lines += lines;
  curr_arg++;

  if (curr_arg == argc)
    break;

  /* end of argument loop: */
  }

  if (!bTotalOnly) {
    if (bStdin)
      printf(fmt_str,lines,"stdin");
    else {
      num_files = m;

      for (n = 0; n < num_files; n++)
        ixs[n] = n;

      if (!bNoSort)
        qsort(ixs,num_files,sizeof (int),compare);

      for (n = 0; n < num_files; n++) {
        printf(fmt_str,
          finfo[ixs[n]].lines,
          finfo[ixs[n]].name);
      }

      putchar(NEWLINE);
    }
  }

  if (!bStdin)
    printf(fmt_str,tot_lines,total_str);

  free(finfo);
  free(ixs);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return finfo[ix2].lines - finfo[ix1].lines;
}
