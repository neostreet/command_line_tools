#include <stdio.h>
#include <stdlib.h>
#include "uncmnt.h"

void main(int argc,char **argv)
{
  FILE *fptr;
  int line_no;
  int line_len;
  int comment_start;
  int comment_stop;
  bool bInComment;
  bool bInDoubleQuote;
  int curly_brace_depth;
  int debug_line_no;

  if (argc != 2) {
    printf("usage: uncmnt filename\n");
    return;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return;
  }

  line_no = 0;
  bInComment = false;
  bInDoubleQuote = false;
  curly_brace_depth = 0;

  for ( ; ; ) {
    if (get_uncommented_line_info(fptr,line,&line_len,MAX_LINE_LEN,
      &comment_start,&comment_stop,
      &bInComment,&bInDoubleQuote,&curly_brace_depth) == -1)
      break;

    if (feof(fptr))
      break;

    line_no++;

    if (line_no == debug_line_no)
      debug_line_no = 0;

    make_uncommented_line(uncommented_line,line,line_len,
      comment_start,comment_stop);

    printf("%s\n",uncommented_line);
  }

  fclose(fptr);
}
