#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define FALSE 0
#define TRUE  1

#ifdef WIN32
#define DELIM_CHAR1 ';'
#define DELIM_CHAR2 '\\'
#else
#define DELIM_CHAR1 ':'
#define DELIM_CHAR2 '/'
#endif

#define BUF_LEN 8192
static char buf[BUF_LEN];

static char usage[] = "usage: envff (-debug) env_var filename\n";

int get_next_directory(char *cpt,int len,int *ix_pt,char **result);

int main(int argc,char **argv)
{
  int curr_arg;
  int bDebug;
  int curr_dir;
  char *cpt;
  int len;
  char *cpt2;
  char *result;
  int ix;
  struct stat statbuf;

  if ((argc != 3) && (argc != 4)) {
    printf(usage);
    return 1;
  }

  bDebug = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = TRUE;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  cpt = getenv(argv[curr_arg]);
  len = strlen(cpt);
  ix = 0;
  curr_dir = 0;

  for ( ; ; ) {
    if (!get_next_directory(cpt,len,&ix,&result))
      break;

    if (bDebug)
      printf("%d: %s\n",curr_dir,result);

    sprintf(buf,"%s%c%s",result,DELIM_CHAR2,argv[curr_arg+1]);

    if (!stat(buf,&statbuf)) {
      cpt2 = ctime(&statbuf.st_mtime);
      cpt2[strlen(cpt2) - 1] = 0;
      printf("%s %10d %s\n",cpt2,statbuf.st_size,buf);
    }

    curr_dir++;
  }

  return 0;
}

int get_next_directory(char *cpt,int len,int *ix_pt,char **result)
{
  int ix;
  int orig_ix;
  int chara;

  ix = *ix_pt;
  orig_ix = ix;

  for ( ; ; ) {
    if (ix >= len)
      break;

    chara = cpt[ix];

    if (!chara || (chara == DELIM_CHAR1))
      break;

    ix++;
  }

  if (ix == orig_ix)
    return FALSE;

  if (chara == DELIM_CHAR1)
    cpt[ix] = 0;

  *ix_pt = ix + 1;
  *result = &cpt[orig_ix];

  return TRUE;
}
