#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char save_line[MAX_LINE_LEN];

static char usage[] = "usage: streak_lite filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int line_no;
  int val;
  int positive_streak;
  int negative_streak;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    sscanf(line,"%d",&val);

    if (line_no == 1) {
      if (!val) {
        negative_streak = 1;
        positive_streak = 0;
      }
      else {
        positive_streak = 1;
        negative_streak = 0;
      }
    }
    else {
      if (!val) {
        if (negative_streak)
          negative_streak++;
        else {
          printf("+%d\t%s\n",positive_streak,&save_line[2]);
          negative_streak = 1;
          positive_streak = 0;
        }
      }
      else {
        if (positive_streak)
          positive_streak++;
        else {
          printf("-%d\t%s\n",negative_streak,&save_line[2]);
          positive_streak = 1;
          negative_streak = 0;
        }
      }
    }

    strcpy(save_line,line);
  }

  if (positive_streak)
    printf("+%d\t%s\n",positive_streak,&save_line[2]);
  else
    printf("-%d\t%s\n",negative_streak,&save_line[2]);

  fclose(fptr);

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
