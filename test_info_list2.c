#include <stdio.h>
#include <string.h>
#include "str_list.h"

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "usage: test_info_list (-no_duplicates) file\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoDuplicates;
  struct info_list list;
  struct info_list_elem *work_elem;
  int ix;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bNoDuplicates = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-no_duplicates"))
      bNoDuplicates = true;
    else
      break;
  }

  if ((argc - curr_arg) != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 2;
  }

  line_no = 0;

  list.num_elems = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (member_of_info_list(&list,line,&ix)) {
      if (get_info_list_elem(&list,ix,&work_elem)) {
        work_elem->int1++;
      }
    }
    else
      add_info_list_elem(&list,line,1,0,0,0,bNoDuplicates);
  }

  work_elem = list.first_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%d %s\n",work_elem->int1,work_elem->str);
    work_elem = work_elem->next_elem;
  }

  free_info_list(&list);

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
