#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 4096
static char line[MAX_LINE_LEN];

static char usage[] = "usage: gaps (-rev) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  bool bReverse;
  FILE *fptr;
  int line_len;
  int line_no;
  int last_val;
  int curr_val;

  if ((argc != 2) && (argc != 3)) {
    printf(usage);
    return 1;
  }

  bReverse = false;

  if (argc == 3) {
    if (!strcmp(argv[1],"-rev"))
      bReverse = true;
    else {
      printf(usage);
      return 2;
    }
  }

  if ((fptr = fopen(argv[argc - 1],"r")) == NULL) {
    printf("couldn't open %s\n",argv[argc - 1]);
    return 3;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    sscanf(line,"%d",&curr_val);

    if (line_no) {
      if (!bReverse) {
        if (curr_val <= last_val) {
          printf("value %d (%d) <= value %d (%d)\n",
            curr_val,line_no,last_val,line_no-1);
          break;
        }
      }
      else {
        if (curr_val >= last_val) {
          printf("value %d (%d) >= value %d (%d)\n",
            curr_val,line_no,last_val,line_no-1);
          break;
        }
      }

      if (!bReverse) {
        if (curr_val != last_val + 1) {
          if (curr_val == last_val + 2)
            printf("%d\n",last_val + 1);
          else
            printf("%d - %d (%d)\n",
              last_val + 1,curr_val - 1,curr_val - last_val - 1);
        }
      }
      else {
        if (curr_val != last_val - 1) {
          if (curr_val == last_val - 2)
            printf("%d\n",last_val - 1);
          else
            printf("%d - %d\n",
              last_val - 1,curr_val + 1,last_val - curr_val - 1);
        }
      }
    }

    last_val = curr_val;
    line_no++;
  }

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
