#include <stdio.h>
#include <string.h>
#include "str_list.h"

static char usage[] = "usage: test_info_list (-no_duplicates) str int int (str int int ...)\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bNoDuplicates;
  struct info_list list;
  struct info_list_elem *work_elem;
  int work1;
  int work2;
  int ix;

  if (argc < 4) {
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

  if ((argc - curr_arg) % 3) {
    printf(usage);
    return 2;
  }

  list.num_elems = 0;

  for ( ; curr_arg < argc; curr_arg += 3) {
    sscanf(argv[curr_arg+1],"%d",&work1);
    sscanf(argv[curr_arg+2],"%d",&work2);

    if (member_of_info_list(&list,argv[curr_arg],&ix)) {
      if (get_info_list_elem(&list,ix,&work_elem)) {
        work_elem->int1 += work1;
        work_elem->int2 += work2;
      }
    }
    else
      add_info_list_elem(&list,argv[curr_arg],work1,work2,bNoDuplicates);
  }

  print_info_list(&list);

  work_elem = list.last_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%s %d %d\n",work_elem->str,
      work_elem->int1,work_elem->int2);
    work_elem = work_elem->prev_elem;
  }

  free_info_list(&list);

  return 0;
}
