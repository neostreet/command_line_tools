#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str_misc.h"
#include "str_list.h"

#define FALSE 0
#define TRUE  1

static char usage[] =
"usage: aggreg2 (-no_sort) offset length filename\n";

static struct info_list aggreg;
static struct info_list_elem **elems;

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoSort;
  int offset;
  int length;
  FILE *fptr;
  int linelen;
  int line_no;
  int ix;
  int dbg_line = 0;
  int dbg = 0;
  int num_elems;
  struct info_list_elem *work_elem;
  int *sort_ixs;
  int tot_lines;

  if ((argc < 4) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bNoSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-no_sort"))
      bNoSort = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&offset);
  sscanf(argv[curr_arg+1],"%d",&length);

  if ((fptr = fopen(argv[curr_arg+2],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg+2]);
    return 3;
  }

  line_no = 0;
  ix = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no == dbg_line)
      dbg++;

    line[offset + length] = 0;

    if (member_of_info_list(&aggreg,&line[offset],&ix)) {
      if (get_info_list_elem(&aggreg,ix,&work_elem))
        work_elem->int1++;
    }
    else
      add_info_list_elem(&aggreg,&line[offset],1,0,0,0,TRUE);
  }

  fclose(fptr);

  num_elems = aggreg.num_elems;
  work_elem = aggreg.first_elem;

  if ((sort_ixs = (int *)malloc(num_elems * sizeof (int))) == NULL) {
    printf("malloc of %d ints failed\n",num_elems);
    return 4;
  }

  if ((elems = (struct info_list_elem **)malloc(num_elems * sizeof (struct info_list_elem *))) == NULL) {
    printf("malloc of %d pointers failed\n",num_elems);
    return 5;
  }

  for (n = 0; n < num_elems; n++) {
    elems[n] = work_elem;
    sort_ixs[n] = n;

    if (n < num_elems - 1)
      work_elem = work_elem->next_elem;
  }

  if (!bNoSort)
    qsort(sort_ixs,num_elems,sizeof (int),elem_compare);

  tot_lines = 0;

  for (n = 0; n < num_elems; n++) {
    printf("%10d %s\n",elems[sort_ixs[n]]->int1,elems[sort_ixs[n]]->str);
    tot_lines += elems[sort_ixs[n]]->int1;
  }

  printf("==========\n");
  printf("%10d\n",tot_lines);

  free(elems);
  free(sort_ixs);

  free_info_list(&aggreg);

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

  return elems[ix2]->int1 - elems[ix1]->int1;
}
