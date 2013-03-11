#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <string.h>

static char usage[] = "usage: skeleton4 (-sort) (-reverse) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static bool bReverse;

static char **cppt;
static int *per_line_int_data;

static char malloc_fail1[] = "malloc of %d chars failed\n";
static char malloc_fail2[] = "malloc of %d char pointers failed\n";
static char malloc_fail3[] = "malloc of %d ints failed\n";

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bSort;
  struct stat statbuf;
  int mem_amount;
  char *mempt;
  FILE *fptr;
  int chara;
  int num_lines;
  int file_ix;
  int *ixs;
  int cppt_ix;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bSort = false;
  bReverse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
    else if (!strcmp(argv[curr_arg],"-reverse"))
      bReverse = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 3;
  }
  else
    mem_amount = (size_t)statbuf.st_size;

  if ((mempt = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_fail1,mem_amount);
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    free(mempt);
    return 5;
  }

  num_lines = 0;

  for (n = 0; ; n++) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n') {
      num_lines++;
      chara = 0;
    }

    mempt[n] = chara;
  }

  fclose(fptr);

  if ((cppt = (char **)malloc(num_lines * sizeof (char *))) == NULL) {
    printf(malloc_fail2,num_lines);
    free(mempt);
    return 6;
  }

  if ((per_line_int_data = (int *)malloc(num_lines * sizeof (int))) == NULL) {
    printf(malloc_fail3,num_lines);
    free(cppt);
    free(mempt);
    return 7;
  }

  if ((ixs = (int *)malloc(num_lines * sizeof (int))) == NULL) {
    printf(malloc_fail3,num_lines);
    free(per_line_int_data);
    free(cppt);
    free(mempt);
    return 8;
  }

  for (n = 0; n < num_lines; n++) {
    ixs[n] = n;
    per_line_int_data[n] = n;
  }

  file_ix = 0;
  cppt_ix = 0;

  for (n = 0; n < mem_amount; n++) {
    if (!mempt[n]) {
      cppt[file_ix++] = &mempt[cppt_ix];
      cppt_ix = n + 1;
    }
  }

  if (bSort)
    qsort(ixs,num_lines,sizeof (int),compare);

  for (n = 0; n < num_lines; n++)
    printf("%10d %s\n",per_line_int_data[ixs[n]],cppt[ixs[n]]);

  free(ixs);
  free(per_line_int_data);
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

  if (per_line_int_data[int1] == per_line_int_data[int2])
    return strcmp(cppt[int1],cppt[int2]);

  if (!bReverse)
    return per_line_int_data[int1] - per_line_int_data[int2];
  else
    return per_line_int_data[int2] - per_line_int_data[int1];
}
