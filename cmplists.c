#include <stdio.h>
#include <string.h>
#include "str_list.h"
 
#define FALSE 0
#define TRUE  1
 
#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
 
static char usage[] =
"usage: cmplists (-not) (-reverse) (-no_dups) list1 list2\n";
static char couldnt_open[] = "couldn't open %s\n";
 
static struct str_list list[2];
 
static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
 
int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bNot;
  int bReverse;
  int bNoDups;
  int bMemberOf;
  FILE *fptr[2];
  int line_len;
  int line_no[2];
  int ix;
  int num_elems;
  struct str_list_elem *work_elem;
 
  if (argc < 3) {
    printf(usage);
    return 1;
  }
 
  bNot = FALSE;
  bReverse = FALSE;
  bNoDups = FALSE;
 
  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-not"))
      bNot = TRUE;
    else if (!strcmp(argv[curr_arg],"-reverse"))
      bReverse = TRUE;
    else if (!strcmp(argv[curr_arg],"-no_dups"))
      bNoDups = TRUE;
    else
      break;
  }
 
  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }
 
  for (n = 0; n < 2; n++) {
    if ((fptr[n] = fopen(argv[curr_arg+n],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg+n]);
      return 3;
    }
 
    line_no[n] = 0;
 
    for ( ; ; ) {
      GetLine(fptr[n],line,&line_len,MAX_LINE_LEN);
 
      if (feof(fptr[n]))
        break;
 
      line_no[n]++;
 
      add_str_list_elem(&list[n],line,bNoDups);
    }
 
    fclose(fptr[n]);
  }
 
  num_elems = list[bReverse].num_elems;
  work_elem = list[bReverse].first_elem;
 
  for (n = 0; n < num_elems; n++) {
    bMemberOf = member_of_str_list(&list[1 - bReverse],work_elem->str,&ix);
 
    if (!bNot) {
      if (bMemberOf)
        printf("%s\n",work_elem->str);
    }
    else {
      if (!bMemberOf)
        printf("%s\n",work_elem->str);
    }
 
    if (n < num_elems - 1)
      work_elem = work_elem->next_elem;
  }
 
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
