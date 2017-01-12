#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str_list.h"

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: blame_authors (-no_sort) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static struct info_list authors;

struct info_list_elem_contig {
  char *str;
  int int1;
};

struct info_list_elem_contig *contig_elems;

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int get_blame_author(char *line,int line_len,char **author);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoSort;
  FILE *fptr;
  int line_len;
  int ix;
  int num_elems;
  struct info_list_elem *work_elem;
  char *author;
  int *ixs;
  int tot_lines;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (get_blame_author(line,line_len,&author)) {
      if (member_of_info_list(&authors,author,&ix)) {
        if (get_info_list_elem(&authors,ix,&work_elem))
          work_elem->int1++;
      }
      else
        add_info_list_elem(&authors,author,1,0,0,0,true);
    }
  }

  fclose(fptr);

  num_elems = authors.num_elems;
  contig_elems = (struct info_list_elem_contig *)malloc(num_elems * sizeof (struct info_list_elem_contig));

  if (contig_elems == NULL) {
    printf("couldn't malloc %d info_list_elem_contig structs\n",num_elems);
    return 4;
  }

  work_elem = authors.first_elem;

  for (n = 0; n < num_elems; n++) {
    contig_elems[n].str = work_elem->str;
    contig_elems[n].int1 = work_elem->int1;

    if (n < num_elems - 1)
      work_elem = work_elem->next_elem;
  }

  ixs = (int *)malloc(num_elems * sizeof (int));

  if (ixs == NULL) {
    printf("couldn't malloc %d ints\n",num_elems);
    return 5;
  }

  for (n = 0; n < num_elems; n++)
    ixs[n] = n;

  if (!bNoSort)
    qsort(ixs,num_elems,sizeof (int),compare);

  tot_lines = 0;

  for (n = 0; n < num_elems; n++) {
    printf("%4d %s\n",contig_elems[ixs[n]].int1,contig_elems[ixs[n]].str);
    tot_lines += contig_elems[ixs[n]].int1;
  }

  printf("\n%4d\n",tot_lines);

  free(ixs);
  free(contig_elems);
  free_info_list(&authors);

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

int get_blame_author(char *line,int line_len,char **author)
{
  int m;
  int n;

  for (n = 0; n < line_len; n++) {
    if (line[n] == '(')
      break;
  }

  if (n >= line_len - 1)
    return 0;

  n++;
  m = n;

  for (n++ ; n < line_len - 1; n++) {
    if ((line[n] == ' ') && ((line[n+1] == ' ') || ((line[n+1] >= '0') && (line[n+1] <= '9')))) {
      line[n] = 0;
      break;
    }
  }

  *author = &line[m];

  return 1;
}

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return contig_elems[ix2].int1 - contig_elems[ix1].int1;
}
