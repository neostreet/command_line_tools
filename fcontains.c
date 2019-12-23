#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
 
#define MAX_SPECIAL_CHARS 20
 
struct pair {
  int val1;
  char val2;
};
 
#define MAX_LINE_LEN 32768
char line[MAX_LINE_LEN];
 
#define TITLELEN 4096
char title[TITLELEN];
 
#define MAX_FILENAME_LEN 256
 
struct name_time {
  char filename[MAX_FILENAME_LEN];
  time_t time;
};
 
#define MAX_FILES 5000
 
static char usage[] = "\
usage: fcontains (-c) (-rev) (-dss) (-special_charhhc)\n\
  (-detail) (-sort) (-file_maxn) string filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";
 
static struct name_time *namtim;
 
void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
 
int elem_compare(const void *elem1,const void *elem2);
 
#ifndef WIN32
#define strcmpi strcmp
#endif
 
int main(int argc,char **argv)
{
  int *ixs;
  int file_ix;
  FILE *fptr0;
  int titlelen;
  FILE *fptr;
  struct stat statbuf;
  char *cpt;
  int chara;
  int linelen;
  int searchlen;
  int line_no;
  int put_title;
  int tries;
  int m;
  int n;
  int p;
  bool bCaseSens;
  bool bReverse;
  bool bDollarSignSwap;
  bool bDetail;
  bool bSort;
  int file_max;
  int curr_arg;
  int put_search_string;
  int put_line;
  int file_count;
  int num_special_chars;
  struct pair special_chars[MAX_SPECIAL_CHARS];
  char buf[3];
 
  if (argc < 3) {
    printf(usage);
    return 1;
  }
 
  bCaseSens = false;
  bReverse = false;
  bDollarSignSwap = false;
  bDetail = false;
  bSort = false;
  num_special_chars = 0;
  file_max = -1;
 
  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmpi(argv[curr_arg],"-c"))
      bCaseSens = true;
    else if (!strcmpi(argv[curr_arg],"-rev"))
      bReverse = true;
    else if (!strcmpi(argv[curr_arg],"-dss"))
      bDollarSignSwap = true;
    else if (!strncmp(argv[curr_arg],"-special_char",13)) {
      if (num_special_chars == MAX_SPECIAL_CHARS) {
        printf("num_special_chars may not exceed %d\n",MAX_SPECIAL_CHARS);
        return 2;
      }
 
      buf[0] = argv[curr_arg][13];
      buf[1] = argv[curr_arg][14];
      buf[2] = 0;
 
      sscanf(buf,"%x",&special_chars[num_special_chars].val1);
     sscanf(&argv[curr_arg][15],"%c",&special_chars[num_special_chars++].val2);
    }
    else if (!strcmpi(argv[curr_arg],"-detail"))
      bDetail = true;
    else if (!strcmpi(argv[curr_arg],"-sort"))
      bSort = true;
    else if (!strncmp(argv[curr_arg],"-file_max",9))
      sscanf(&argv[curr_arg][9],"%d",&file_max);
    else
      break;
  }
 
  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }
 
  if ((fptr0 = fopen(argv[argc - 1],"r")) == NULL) {
    printf(couldnt_open,argv[argc - 1]);
    return 4;
  }
 
  if (bSort) {
    if ((namtim = (struct name_time *)malloc(
      MAX_FILES * sizeof (struct name_time))) == NULL) {
      printf("malloc of namtim failed\n");
      return 5;
    }
 
    if ((ixs = (int *)malloc(
      MAX_FILES * sizeof (int))) == NULL) {
      printf("malloc of ixs failed\n");
      return 6;
    }
 
    file_ix = 0;
  }
 
  if (num_special_chars || !bCaseSens || bDollarSignSwap) {
    for (n = 0; (chara = argv[argc - 2][n]); n++) {
      for (p = 0; p < num_special_chars; p++) {
        if (chara == special_chars[p].val2) {
          argv[argc - 2][n] = (char)special_chars[p].val1;
          break;
        }
      }
 
      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          argv[argc - 2][n] = (char)(chara - 'A' + 'a');
      }
 
      if (bDollarSignSwap) {
        if (chara == ':')
          argv[argc - 2][n] = '$';
      }
    }
  }
 
  searchlen = strlen(argv[argc - 2]);
  put_search_string = 0;
 
  if (file_max != -1)
    file_count = 0;
 
  for ( ; ; ) {
    GetLine(fptr0,title,&titlelen,TITLELEN);
 
    if (feof(fptr0))
      break;
 
    if (stat(title,&statbuf)) {
      printf(couldnt_get_status,title);
 
      continue;
    }
 
    if (file_max != -1) {
      file_count++;
 
      if (file_count > file_max)
        break;
    }
 
    if ((fptr = fopen(title,"r")) == NULL) {
      printf(couldnt_open,title);
 
      continue;
    }
 
    put_title = 0;
 
    if (!bReverse)
      put_line = 0;
    else
      put_line = 1;
 
    for (line_no = 1; ; line_no++) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);
 
      if (feof(fptr)) {
        if (put_title)
          putchar(0x0a);
 
        fclose(fptr);
 
        break;
      }
 
      if (linelen >= searchlen) {
        tries = linelen - searchlen + 1;
 
        for (m = 0; m < tries; m++) {
          for (n = 0; n < searchlen; n++) {
            chara = line[m+n];
 
            if (!bCaseSens)
              if ((chara >= 'A') && (chara <= 'Z'))
                chara -= 'A' - 'a';
 
            if (chara != argv[argc - 2][n])
              break;
          }
 
          if (n == searchlen)
            break;
        }
 
        if (n == searchlen) {
          if (!bReverse)
            put_line = 1;
          else
            put_line = 0;
        }
      }
 
      if (!bReverse && put_line) {
        if (!put_search_string) {
          if (bDetail)
            printf("search string: %s\n\n",argv[argc - 2]);
 
          put_search_string = 1;
        }
 
        if (!put_title) {
          if ((bSort) && (file_ix == MAX_FILES)) {
            printf("warning: limit of MAX_FILES reached\n");
            break;
          }
          else {
            if (!bSort)
              printf("%s\n",title);
            else {
              strcpy(namtim[file_ix].filename,title);
              namtim[file_ix].time = statbuf.st_mtime;
              file_ix++;
            }
          }
 
          put_title = 1;
        }
 
        fclose(fptr);
 
        break;
      }
 
      if (bReverse && !put_line) {
        fclose(fptr);
 
        break;
      }
    }
 
    if (bReverse && put_line) {
      if (!put_search_string) {
        if (bDetail)
          printf("search string: %s\n\n",argv[argc - 2]);
 
        put_search_string = 1;
      }
 
      if (!put_title) {
        if ((bSort) && (file_ix == MAX_FILES))
          printf("warning: limit of MAX_FILES reached\n");
        else {
          if (!bSort)
            printf("%s\n",title);
          else {
            strcpy(namtim[file_ix].filename,title);
            namtim[file_ix].time = statbuf.st_mtime;
            file_ix++;
          }
        }
 
        put_title = 1;
      }
    }
  }
 
  fclose(fptr0);
 
  if (!bSort)
    return 0;
 
  for (n = 0; n < file_ix; n++)
    ixs[n] = n;
 
  qsort(ixs,file_ix,sizeof (int),elem_compare);
 
  for (n = 0; n < file_ix; n++) {
    if (bDetail) {
      cpt = ctime(&namtim[ixs[n]].time);
      cpt[strlen(cpt) - 1] = 0;
      printf("%s %s\n",cpt,namtim[ixs[n]].filename);
    }
    else
      printf("%s\n",namtim[ixs[n]].filename);
  }
 
  free(namtim);
 
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
 
int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;
 
  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;
 
  return namtim[ix2].time - namtim[ix1].time;
}
