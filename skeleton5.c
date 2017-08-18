#include <stdio.h>
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <malloc.h>
#include <string.h>

static char usage[] = "usage: skeleton5 filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char **cppt;

static char malloc_fail1[] = "malloc of %d chars failed\n";
static char malloc_fail2[] = "malloc of %d char pointers failed\n";

int main(int argc,char **argv)
{
  int n;
  struct stat statbuf;
  int mem_amount;
  char *mempt;
  FILE *fptr;
  int chara;
  int num_lines;
  int file_ix;
  int cppt_ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }
  else
    mem_amount = (size_t)statbuf.st_size;

  if ((mempt = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_fail1,mem_amount);
    return 3;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    free(mempt);
    return 4;
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
    return 5;
  }

  file_ix = 0;
  cppt_ix = 0;

  for (n = 0; n < mem_amount; n++) {
    if (!mempt[n]) {
      cppt[file_ix++] = &mempt[cppt_ix];
      cppt_ix = n + 1;
    }
  }

  for (n = 0; n < num_lines; n++)
    printf("%s\n",cppt[n]);

  free(cppt);
  free(mempt);

  return 0;
}
