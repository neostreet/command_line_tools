#include <stdio.h>
#include "str_list.h"

#define FALSE 0
#define TRUE  1

int main(int argc,char **argv)
{
  int n;
  struct str_list list;
  struct str_list_elem *work_elem;

  if (argc < 2) {
    printf("usage: test_str_list str (str ...)\n");
    return 1;
  }

  list.num_elems = 0;

  for (n = 1; n < argc; n++) {
    add_str_list_elem(&list,argv[n],TRUE);
  }

  print_str_list(&list);

  work_elem = list.last_elem;

  for (n = 0; n < list.num_elems; n++) {
    printf("%s\n",work_elem->str);
    work_elem = work_elem->prev_elem;
  }

  free_str_list(&list);

  return 0;
}
