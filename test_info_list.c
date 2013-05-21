#include <stdio.h>
#include <string.h>
#include "str_list.h"

static char usage[] =
"usage: test_info_list (-no_duplicates) str int int int (str int int int ...)\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoDuplicates;
  struct info_list list;
  struct info_list_elem *work_elem;
  int work1;
  int work2;
  int work3;
  int ix;

  if (argc < 5) {
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

  if ((argc - curr_arg) % 4) {
    printf(usage);
    return 2;
  }

  list.num_elems = 0;

  for ( ; curr_arg < argc; curr_arg += 4) {
    sscanf(argv[curr_arg+1],"%d",&work1);
    sscanf(argv[curr_arg+2],"%d",&work2);
    sscanf(argv[curr_arg+3],"%d",&work3);

    if (member_of_info_list(&list,argv[curr_arg],&ix)) {
      if (get_info_list_elem(&list,ix,&work_elem)) {
        work_elem->int1 += work1;
        work_elem->int2 += work2;
        work_elem->int3 += work3;
      }
    }
    else
      add_info_list_elem(&list,argv[curr_arg],work1,work2,work3,bNoDuplicates);
  }

  work_elem = list.first_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%s %d %d %d\n",work_elem->str,
      work_elem->int1,work_elem->int2,work_elem->int3);
    work_elem = work_elem->next_elem;
  }

  work_elem = list.last_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%s %d %d %d\n",work_elem->str,
      work_elem->int1,work_elem->int2,work_elem->int3);
    work_elem = work_elem->prev_elem;
  }

  free_info_list(&list);

  return 0;
}
