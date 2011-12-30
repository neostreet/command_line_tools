#include <stdio.h>
#include "str_list.h"

#define FALSE 0
#define TRUE  1

static char usage[] = "\
usage: test_info2_list str int int int void_ptr (str int int int void_ptr ...)\n";

int main(int argc,char **argv)
{
  int n;
  struct info2_list list;
  struct info2_list_elem *work_elem;
  int work1;
  int work2;
  int work3;

  if (argc < 6) {
    printf(usage);
    return 1;
  }

  if ((argc - 1) % 5) {
    printf(usage);
    return 2;
  }

  list.num_elems = 0;

  for (n = 1; n < argc; n += 5) {
    sscanf(argv[n+1],"%d",&work1);
    sscanf(argv[n+2],"%d",&work2);
    sscanf(argv[n+3],"%d",&work3);
    add_info2_list_elem(&list,argv[n],work1,work2,work3,argv[n+3]);
  }

  print_info2_list(&list);

  work_elem = list.last_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%s %d %d %d %p\n",work_elem->str,
      work_elem->int1,work_elem->int2,work_elem->int3,
      work_elem->void_ptr);
    work_elem = work_elem->prev_elem;
  }

  free_info2_list(&list);

  return 0;
}
