#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

static char couldnt_get_status[] = "couldn't get status of %s\n";

#define MAX_FILES 30000

#define MAX_LINE_LEN 128
static char line[MAX_LINE_LEN];

struct file_info {
  char name[MAX_LINE_LEN];
  time_t stmtime;
  off_t stsize;
  int cksum;
};

static struct file_info *finfo;

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);
int get_cksum(char *filename);

int main(int argc,char **argv)
{
  int n;
  bool bStdin;
  int linelen;
  struct stat statbuf;
  char *cpt1;
  char *cpt2;
  int curr_file;
  int num_files;
  int *ixs;
  bool bNoSort;
  bool bNoDate;
  bool bCksum;

  finfo = (struct file_info *)malloc(MAX_FILES * sizeof (struct file_info));

  if (finfo == NULL) {
    printf("couldn't malloc %d bytes\n",MAX_FILES * sizeof (struct file_info));
    return 1;
  }

  ixs = (int *)malloc(MAX_FILES * sizeof (int));

  if (ixs == NULL) {
    printf("couldn't malloc %d bytes\n",MAX_FILES * sizeof (int));
    return 2;
  }

  bNoSort = false;
  bNoDate = false;
  bCksum = false;

  for (n = 1; n < argc; n++) {
    if (!strcmp(argv[n],"-no_sort"))
      bNoSort = true;
    else if (!strcmp(argv[n],"-no_date"))
      bNoDate = true;
    else if (!strcmp(argv[n],"-cksum"))
      bCksum = true;
    else
      break;
  }

  if (argc - (bNoSort + bNoDate + bCksum) == 1) {
    bStdin = true;
    cpt1 = line;
  }
  else {
    bStdin = false;
    n = bNoSort + bNoDate + 1;
  }

  curr_file = 0;

  for ( ; ; ) {
    if (bStdin) {
      GetLine(stdin,line,&linelen,MAX_LINE_LEN);

      if (feof(stdin))
        break;
    }
    else
      cpt1 = argv[n];

    if (curr_file == MAX_FILES) {
      printf("maximum of %d files reached\n",MAX_FILES);
      break;
    }

    if (stat(cpt1,&statbuf)) {
      printf(couldnt_get_status,cpt1);

      if (!bStdin) {
        n++;

        if (n == argc)
          break;
      }

      continue;
    }

    strcpy(finfo[curr_file].name,cpt1);
    finfo[curr_file].stmtime = statbuf.st_mtime;
    finfo[curr_file].stsize = statbuf.st_size;

    if (bCksum)
      finfo[curr_file].cksum = get_cksum(cpt1);

    curr_file++;

    if (!bStdin) {
      n++;

      if (n == argc)
        break;
    }
  }

  num_files = curr_file;

  for (n = 0; n < num_files; n++)
    ixs[n] = n;

  if (!bNoSort)
    qsort(ixs,num_files,sizeof (int),compare);

  for (n = 0; n < num_files; n++) {
    if (!bNoDate) {
      cpt2 = ctime(&finfo[ixs[n]].stmtime);
      cpt2[strlen(cpt2) - 1] = 0;

      if (bCksum)
        printf("%s %10d %08x %s\n",cpt2,finfo[ixs[n]].stsize,
          finfo[ixs[n]].cksum,finfo[ixs[n]].name);
      else
        printf("%s %10d %s\n",cpt2,finfo[ixs[n]].stsize,
          finfo[ixs[n]].name);
    }
    else
      printf("%s\n",finfo[ixs[n]].name);
  }

  free(finfo);
  free(ixs);

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return finfo[ix2].stmtime - finfo[ix1].stmtime;
}

int get_cksum(char *filename)
{
  FILE *fptr;
  int cksum;
  int chara;

  if ((fptr = fopen(filename,"r")) == NULL)
    return 0;

  cksum = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    cksum += chara;
  }

  fclose(fptr);

  return cksum;
}
