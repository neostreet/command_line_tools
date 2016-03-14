#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: gen_compares1 filename\n";
static char couldnt_open[] = "couldn't open %s\n";

struct left_top {
  int left;
  int top;
};

#define NUM_LEFT_TOPS 20
static struct left_top left_tops[NUM_LEFT_TOPS];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  printf("@echo off\n");

  line_no = 0;

  for (n = 0; n < NUM_LEFT_TOPS; n++) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d %d",&left_tops[n].left,&left_tops[n].top);
  }

  fclose(fptr);

  for (m = 0; m < NUM_LEFT_TOPS - 1; m++) {
    for (n = m + 1; n < NUM_LEFT_TOPS; n++) {
      printf("compare_26_2 -debug 2b 3b 4b 5b 6b 7b 8b 9b Tb Jb Qb Kb Ab 2r 3r 4r 5r 6r 7r 8r 9r Tr Jr Qr Kr Ar %d %d %d %d > compare_26_2_debug_%d_%d_%d_%d.out\n",
        left_tops[m].left,left_tops[m].top,left_tops[n].left,left_tops[n].top,
        left_tops[m].left,left_tops[m].top,left_tops[n].left,left_tops[n].top);
    }
  }

  printf("call mydl out\n");

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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
