#include <stdio.h>
#include "str_list.h"

#define BUF_LEN 1024
static char buf[BUF_LEN];

static struct str_list player_list;

int main(int argc,char **argv)
{
  int n;
  int ix;
  struct str_list_elem *work_elem;

  if (argc != 2) {
    printf("usage: test_str_list3 ix\n");
    return 1;
  }

  sscanf(argv[1],"%d",&ix);

  if ((ix < 0) || (ix > 9)) {
    printf("ix must be >= 0 and <= 9\n");
    return 2;
  }

  for (n = 0; n < 9; n++) {
    sprintf(buf,"player%d",n);
    add_str_list_elem(&player_list,buf,false);
  }

  print_str_list(&player_list);

  work_elem = player_list.last_elem;

  for (n = 0; n < player_list.num_elems; n++) {
    printf("%s\n",work_elem->str);
    work_elem = work_elem->prev_elem;
  }

  putchar(0x0a);

  remove_str_list_elem(&player_list,ix);
  print_str_list(&player_list);

  work_elem = player_list.last_elem;

  for (n = 0; n < player_list.num_elems; n++) {
    printf("%s\n",work_elem->str);
    work_elem = work_elem->prev_elem;
  }

  free_str_list(&player_list);

  return 0;
}
