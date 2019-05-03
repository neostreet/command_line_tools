#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str_misc.h"
#include "str_list.h"

#define FALSE 0
#define TRUE  1

static char usage[] = "usage: aggreg (-delim_charc) filename\n";

static char sample_date_str[] = "Mon Dec 23 18:22:24 2002 ";
#define DATE_STR_LEN (sizeof sample_date_str - 1)

static char *day_strs[] = {
  (char *)"Sun ", (char *)"Mon ", (char *)"Tue ",
  (char *)"Wed ", (char *)"Thu ", (char *)"Fri ",
  (char *)"Sat "
};
#define NUM_DAYS (sizeof day_strs / sizeof (char **))

static struct info_list aggreg;
static struct info_list_elem **elems;

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

char *GetLine(FILE *fptr,char *line,int maxllen);
int elem_compare(const void *elem1,const void *elem2);
void delimit_line(char *line,char delim_char);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int bDelim;
  char delim_char;
  FILE *fptr;
  char *cpt;
  int line_no;
  int ix;
  int dbg_line = 0;
  int dbg = 0;
  int num_elems;
  struct info_list_elem *work_elem;
  int *sort_ixs;
  int tot_lines;

  if ((argc != 2) && (argc != 3)) {
    printf(usage);
    return 1;
  }

  bDelim = FALSE;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-delim_char",11)) {
      bDelim = TRUE;
      sscanf(&argv[curr_arg][11],"%c",&delim_char);
    }
    else
      break;
  }

  if (curr_arg != argc - 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf("couldn't open %s\n",argv[curr_arg]);
    return 3;
  }

  line_no = 0;
  ix = 0;

  for ( ; ; ) {
    cpt = GetLine(fptr,line,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no == dbg_line)
      dbg++;

    if (bDelim)
      delimit_line(cpt,delim_char);

    if (member_of_info_list(&aggreg,cpt,&ix)) {
      if (get_info_list_elem(&aggreg,ix,&work_elem))
        work_elem->int1++;
    }
    else
      add_info_list_elem(&aggreg,cpt,1,0,0,0,TRUE);
  }

  fclose(fptr);

  num_elems = aggreg.num_elems;
  work_elem = aggreg.first_elem;

  if ((sort_ixs = (int *)malloc(num_elems * sizeof (int))) == NULL) {
    printf("malloc of %d ints failed\n",num_elems);
    return 4;
  }

  if ((elems = (info_list_elem **)malloc(num_elems * sizeof (struct info_list_elem *))) == NULL) {
    printf("malloc of %d pointers failed\n",num_elems);
    return 5;
  }

  for (n = 0; n < num_elems; n++) {
    elems[n] = work_elem;
    sort_ixs[n] = n;

    if (n < num_elems - 1)
      work_elem = work_elem->next_elem;
  }

  qsort(sort_ixs,num_elems,sizeof (int),elem_compare);

  tot_lines = 0;

  for (n = 0; n < num_elems; n++) {
    printf("%5d %s\n",elems[sort_ixs[n]]->int1,elems[sort_ixs[n]]->str);
    tot_lines += elems[sort_ixs[n]]->int1;
  }

  printf("=====\n");
  printf("%5d\n",tot_lines);

  free(elems);
  free(sort_ixs);

  free_info_list(&aggreg);

  return 0;
}

char *GetLine(FILE *fptr,char *line,int maxllen)
{
  int n;
  int chara;
  int line_len;

  line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (line_len < maxllen - 1)
      line[line_len++] = (char)chara;
  }

  line[line_len] = 0;

  for (n = 0; n < NUM_DAYS; n++) {
    if (!strncmp(line,day_strs[n],4))
      break;
  }

  if (n < NUM_DAYS)
    return &line[DATE_STR_LEN];
  else
    return &line[0];
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return elems[ix2]->int1 - elems[ix1]->int1;
}

void delimit_line(char *line,char delim_char)
{
  int n;

  for (n = 0; n < (line[n]); n++) {
    if (line[n] == delim_char) {
      line[n] = 0;
      return;
    }
  }
}
