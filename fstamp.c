#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <string.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

static char usage[] = "usage: fstamp (-no_sort) filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char **cppt;

struct file_info {
  time_t st_mtime;
  off_t st_size;
};

static struct file_info *finfo;

static char malloc_fail1[] = "malloc of %d chars failed\n";
static char malloc_fail2[] = "malloc of %d char pointers failed\n";
static char malloc_fail3[] = "malloc of %d ints failed\n";

int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bNoSort;
  struct stat statbuf;
  int mem_amount;
  char *mempt;
  FILE *fptr;
  int chara;
  int num_lines;
  int file_ix;
  int *ixs;
  int cppt_ix;
  char *cpt;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bNoSort = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-no_sort"))
      bNoSort = TRUE;
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

  if ((finfo = (struct file_info *)malloc(num_lines * sizeof (struct file_info))) == NULL) {
    printf(malloc_fail3,num_lines);
    free(cppt);
    free(mempt);
    return 7;
  }

  if ((ixs = (int *)malloc(num_lines * sizeof (int))) == NULL) {
    printf(malloc_fail3,num_lines);
    free(finfo);
    free(cppt);
    free(mempt);
    return 8;
  }

  for (n = 0; n < num_lines; n++)
    ixs[n] = n;

  file_ix = 0;
  cppt_ix = 0;

  for (n = 0; n < mem_amount; n++) {
    if (!mempt[n]) {
      cppt[file_ix] = &mempt[cppt_ix];
      cppt_ix = n + 1;

      if (stat(cppt[file_ix],&statbuf) == -1) {
        printf(couldnt_get_status,cppt[file_ix]);
        free(ixs);
        free(finfo);
        free(cppt);
        free(mempt);
        return 9;
      }

      finfo[file_ix].st_mtime = statbuf.st_mtime;
      finfo[file_ix].st_size = statbuf.st_size;

      file_ix++;
    }
  }

  if (!bNoSort)
    qsort(ixs,num_lines,sizeof (int),compare);

  for (n = 0; n < num_lines; n++) {
    cpt = ctime(&finfo[ixs[n]].st_mtime);
    cpt[strlen(cpt) - 1] = 0;

    printf("%s %10d %s\n",cpt,finfo[ixs[n]].st_size,
      cppt[ixs[n]]);
  }

  free(ixs);
  free(finfo);
  free(cppt);
  free(mempt);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  if (finfo[ix2].st_mtime == finfo[ix1].st_mtime)
    return strcmp(cppt[ix1], cppt[ix2]);

  return finfo[ix2].st_mtime - finfo[ix1].st_mtime;
}
