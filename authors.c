#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str_list.h"

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] =
"usage: authors (-no_sort) (filename)\n";
static char couldnt_open[] = "couldn't open %s\n";

static char author_str[] = "Author: ";
#define AUTHOR_STR_LEN (sizeof (author_str) - 1)

static struct info_list authors;

struct info_list_elem_contig {
  char *str;
  int int1;
};

struct info_list_elem_contig *contig_elems;

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int get_author(char *line,int line_len,char **author);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoSort;
  bool bStdin;
  FILE *fptr;
  int line_len;
  int ix;
  int num_elems;
  struct info_list_elem *work_elem;
  char *author;
  int *ixs;
  int tot_commits;

  if (argc > 3) {
    printf(usage);
    return 1;
  }

  bNoSort = false;
  bStdin = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-no_sort"))
      bNoSort = true;
    else
      break;
  }

  if (curr_arg == argc)
    bStdin = true;

  if (!bStdin) {
    if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg]);
      return 2;
    }
  }
  else
    fptr = stdin;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (get_author(line,line_len,&author)) {
      if (member_of_info_list(&authors,author,&ix)) {
        if (get_info_list_elem(&authors,ix,&work_elem))
          work_elem->int1++;
      }
      else
        add_info_list_elem(&authors,author,1,0,0,0,true);
    }
  }

  if (!bStdin)
    fclose(fptr);

  num_elems = authors.num_elems;
  contig_elems = (struct info_list_elem_contig *)malloc(num_elems * sizeof (struct info_list_elem_contig));

  if (contig_elems == NULL) {
    printf("couldn't malloc %d info_list_elem_contig structs\n",num_elems);
    return 3;
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
    return 4;
  }

  for (n = 0; n < num_elems; n++)
    ixs[n] = n;

  if (!bNoSort)
    qsort(ixs,num_elems,sizeof (int),compare);

  tot_commits = 0;

  for (n = 0; n < num_elems; n++) {
    printf("%3d %s\n",contig_elems[ixs[n]].int1,contig_elems[ixs[n]].str);
    tot_commits += contig_elems[ixs[n]].int1;
  }

  printf("\n%3d\n",tot_commits);

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

int get_author(char *line,int line_len,char **author)
{
  int n;

  if (line_len < AUTHOR_STR_LEN)
    return 0;

  if (strncmp(line,author_str,AUTHOR_STR_LEN))
    return 0;

  for (n = AUTHOR_STR_LEN; n < line_len; n++) {
    if (line[n] == '<') {
      line[n - 1] = 0;
      break;
    }
  }

  *author = &line[AUTHOR_STR_LEN];

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
