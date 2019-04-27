#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <string.h>

#ifndef WIN32
#define strcmpi strcmp
#endif

#define FNAMELEN 128
char filename[FNAMELEN];

static char usage[] = "usage: flines (-sort) (-no_align) filename\n";

static char couldnt_open[] = "couldn't open %s\n";

static char **cppt;
static int *num_lines;

static char malloc_fail1[] = "malloc of %d chars failed\n";
static char malloc_fail2[] = "malloc of %d char pointers failed\n";
static char malloc_fail3[] = "malloc of %d ints failed\n";

struct file_info {
  int lines;
};

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bSort;
  bool bNoAlign;
  struct stat statbuf;
  int mem_amount;
  char *mempt;
  FILE *fptr0;
  int num_files;
  int file_ix;
  int *ixs;
  int cppt_ix;
  FILE *fptr;
  long tot_count;
  long count;
  int chara;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bSort = false;
  bNoAlign = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else if (!strcmp(argv[curr_arg],"-no_align"))
      bNoAlign = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf("couldn't get status of %s\n",argv[curr_arg]);
    return 3;
  }
  else
    mem_amount = (size_t)statbuf.st_size;

  if ((mempt = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_fail1,mem_amount);
    return 4;
  }

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    free(mempt);
    return 5;
  }

  num_files = 0;

  for (n = 0; ; n++) {
    chara = fgetc(fptr0);

    if (feof(fptr0))
      break;

    if (chara == '\n') {
      num_files++;
      chara = 0;
    }

    mempt[n] = chara;
  }

  if ((cppt = (char **)malloc(num_files * sizeof (char *))) == NULL) {
    printf(malloc_fail2,num_files);
    free(mempt);
    return 6;
  }

  if ((num_lines = (int *)malloc(num_files * sizeof (int))) == NULL) {
    printf(malloc_fail3,num_files);
    free(cppt);
    free(mempt);
    return 7;
  }

  if ((ixs = (int *)malloc(num_files * sizeof (int))) == NULL) {
    printf(malloc_fail3,num_files);
    free(num_lines);
    free(cppt);
    free(mempt);
    return 8;
  }

  for (n = 0; n < num_files; n++)
    ixs[n] = n;

  file_ix = 0;
  cppt_ix = 0;

  for (n = 0; n < mem_amount; n++) {
    if (!mempt[n]) {
      cppt[file_ix++] = &mempt[cppt_ix];
      cppt_ix = n + 1;
    }
  }

  tot_count = 0;

  for (n = 0; n < num_files; n++) {
    if ((fptr = fopen(cppt[n],"r")) == NULL) {
      printf("couldn't open %s\n",cppt[n]);
      num_lines[n] = 0;
      continue;
    }

    count = 0;

    for ( ; ; ) {
      chara = fgetc(fptr);

      if (feof(fptr))
        break;

      if (chara == '\n')
        count++;
    }

    fclose(fptr);

    num_lines[n] = count;

    tot_count += count;
  }

  if (bSort)
    qsort(ixs,num_files,sizeof (int),compare);

  for (n = 0; n < num_files; n++) {
    if (!bNoAlign)
      printf("%10d %s\n",num_lines[ixs[n]],cppt[ixs[n]]);
    else
      printf("%d %s\n",num_lines[ixs[n]],cppt[ixs[n]]);
  }

  printf("\n%10d TOTAL\n",tot_count);

  free(ixs);
  free(num_lines);
  free(cppt);
  free(mempt);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int int1;
  int int2;

  int1 = *(int *)elem1;
  int2 = *(int *)elem2;

  if (num_lines[int1] == num_lines[int2])
    return strcmp(cppt[int1],cppt[int2]);

  return num_lines[int2] - num_lines[int1];
}
