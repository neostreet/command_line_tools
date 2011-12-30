#include <stdio.h>

#define LINE_FEED 0x0a

void main(int argc,char **argv)
{
  FILE *fptr;
  int chara;
  int last_char;
  int col;
  int bInComment;
  int bCommentInCol1;
  int bLastCharEndedComment;

  if (argc != 2) {
    printf("usage: zapcmmnt filename\n");
    return;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[1]);
    return;
  }

  last_char = 0;
  col = 0;
  bInComment = 0;
  bCommentInCol1 = 0;
  bLastCharEndedComment = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    col++;

    if (bInComment) {
      if ((last_char == '*') && (chara == '/')) {
        bInComment = 0;
        last_char = 0;
        bLastCharEndedComment = 1;
        continue;
      }
    }
    else {
      if (last_char == '/') {
        if (chara == '*') {
          bInComment = 1;

          if (col == 2)
            bCommentInCol1 = 1;
          else
            bCommentInCol1 = 0;
        }
        else
          putchar('/');
      }

      if (!bInComment) {
        if (chara != '/') {
          if (bLastCharEndedComment) {
            if (chara == LINE_FEED) {
              if (!bCommentInCol1)
                putchar(chara);
            }
            else
              putchar(chara);
          }
          else
            putchar(chara);
        }
      }
    }

    last_char = chara;

    if (last_char == LINE_FEED)
      col = 0;

    bLastCharEndedComment = 0;
  }

  fclose(fptr);
}
