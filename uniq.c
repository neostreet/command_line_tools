#include <stdio.h>
#include "str_list.h"

#define MAX_LINE_LEN 1024
char line[MAX_LINE_LEN];

static char usage[] = "usage: uniq filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static struct info_list unique_lines;

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int linelen;
  int ix;
  int num_elems;
  struct info_list_elem *work_elem;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  for ( ; ; ) {
    GetLine(fptr,line,&linelen,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (member_of_info_list(&unique_lines,line,&ix)) {
      if (get_info_list_elem(&unique_lines,ix,&work_elem))
        work_elem->int1++;
    }
    else
      add_info_list_elem(&unique_lines,line,1,0,true);
  }

  fclose(fptr);

  num_elems = unique_lines.num_elems;
  work_elem = unique_lines.first_elem;

  for (n = 0; n < num_elems; n++) {
    printf("%s\n",work_elem->str);

    if (n < num_elems - 1)
      work_elem = work_elem->next_elem;
  }

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
