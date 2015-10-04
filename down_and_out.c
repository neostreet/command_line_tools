#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#else
#define _MAX_PATH 4096
#include <unistd.h>
#endif

static char save_dir[_MAX_PATH];

static char usage[] =
"usage: down_and_out (-get_date_from_path) filename\n";

static int *vals;

int compare(const void *elem1,const void *elem2);
static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bGetDateFromPath;
  int ix;
  FILE *fptr;
  int retval;
  char *date_string;
  int num_vals;
  int work;
  int *ixs;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bGetDateFromPath = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
   if (!strcmp(argv[curr_arg],"-get_date_from_path"))
      bGetDateFromPath = true;
    else
      break;
  }

  if (bGetDateFromPath)
    getcwd(save_dir,_MAX_PATH);

  if (bGetDateFromPath) {
    retval = get_date_from_path(save_dir,'/',2,&date_string);

    if (retval) {
      printf("get_date_from_path() on %s failed: %d\n",save_dir,retval);
      return 2;
    }
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 4;
  }

  num_vals = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    num_vals++;
  }

  if ((vals = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d structs\n",num_vals);
    return 5;
  }

  if ((ixs = (int *)malloc(num_vals * sizeof (int))) == NULL) {
    printf("couldn't malloc %d ints\n",num_vals);
    free(vals);
    return 9;
  }

  fseek(fptr,0L,SEEK_SET);

  ix = 0;

  for ( ; ; ) {
    fscanf(fptr,"%d",&work);

    if (feof(fptr))
      break;

    vals[ix++] = work;
  }

  fclose(fptr);

  for (n = 0; n < num_vals; n++)
    ixs[n] = n;

  qsort(ixs,num_vals,sizeof (int),compare);

  if ((vals[ixs[0]] < 0) && (ixs[0] == num_vals - 1)) {
    if (!bGetDateFromPath)
      printf("%d\n",vals[ixs[0]]);
    else
      printf("%d %s\n",vals[ixs[0]],save_dir);
  }

  free(ixs);
  free(vals);

  return 0;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;
  int int1;
  int int2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  int1 = vals[ix1];
  int2 = vals[ix2];

  return int1 - int2;
}

static char sql_date_string[11];

static int get_date_from_path(char *path,char slash_char,int num_slashes,char **date_string_ptr)
{
  int n;
  int len;
  int slash_count;

  len = strlen(path);
  slash_count = 0;

  for (n = len - 1; (n >= 0); n--) {
    if (path[n] == slash_char) {
      slash_count++;

      if (slash_count == num_slashes)
        break;
    }
  }

  if (slash_count != num_slashes)
    return 1;

  if (path[n+5] != slash_char)
    return 2;

  strncpy(sql_date_string,&path[n+1],4);
  sql_date_string[4] = '-';
  strncpy(&sql_date_string[5],&path[n+6],2);
  sql_date_string[7] = '-';
  strncpy(&sql_date_string[8],&path[n+8],2);
  sql_date_string[10] = 0;

  *date_string_ptr = sql_date_string;

  return 0;
}
