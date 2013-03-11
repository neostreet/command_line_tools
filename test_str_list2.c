#include <stdio.h>
#include "str_list.h"

#define MAX_FILENAME_LEN 1024
char include_filename[MAX_FILENAME_LEN];

int main(int argc,char **argv)
{
  int n;
  int from_ix;
  int ix;
  struct str_list list;
  struct str_list_elem *work_elem;

  if (argc < 4) {
    printf("usage: test_str_list find_str from_ix str (str ...)\n");
    return 1;
  }

  sscanf(argv[2],"%d",&from_ix);

  list.num_elems = 0;

  for (n = 3; n < argc; n++) {
    add_str_list_elem(&list,argv[n],true);
  }

  printf("%d\n",member_of_str_list_from_ix(&list,from_ix,argv[1],&ix));

  free_str_list(&list);

  return 0;
}
