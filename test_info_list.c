#include <stdio.h>
#include "str_list.h"

static char usage[] = "usage: test_info_list str int int (str int int ...)\n";

int main(int argc,char **argv)
{
  int n;
  struct info_list list;
  struct info_list_elem *work_elem;
  int work1;
  int work2;

  if (argc < 4) {
    printf(usage);
    return 1;
  }

  if ((argc - 1) % 3) {
    printf(usage);
    return 2;
  }

  list.num_elems = 0;

  for (n = 1; n < argc; n += 3) {
    sscanf(argv[n+1],"%d",&work1);
    sscanf(argv[n+2],"%d",&work2);
    add_info_list_elem(&list,argv[n],work1,work2,true);
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
