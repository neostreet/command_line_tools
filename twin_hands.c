#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char prev_line[MAX_LINE_LEN];

static char usage[] =
"usage: twin_hands (-line_no) (-first_only) (-second_only) offset len filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bLineNo;
  bool bFirstOnly;
  bool bSecondOnly;
  int offset;
  int len;
  FILE *fptr;
  int line_len;
  int line_no;

  if ((argc < 4) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bLineNo = false;
  bFirstOnly = false;
  bSecondOnly = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-line_no"))
      bLineNo = true;
    else if (!strcmp(argv[curr_arg],"-first_only"))
      bFirstOnly = true;
    else if (!strcmp(argv[curr_arg],"-second_only"))
      bSecondOnly = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  if (bFirstOnly && bSecondOnly) {
    printf("can't specify both -first_only and -second_only\n");
    return 3;
  }

  sscanf(argv[curr_arg],"%d",&offset);
  sscanf(argv[curr_arg+1],"%d",&len);

  if ((fptr = fopen(argv[curr_arg+2],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+2]);
    return 4;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    if (line_no > 1) {
      if (!strncmp(&prev_line[offset],&line[offset],len)) {
        if (!bLineNo) {
          if (bFirstOnly)
            printf("%s\n",prev_line);
          else if (bSecondOnly)
            printf("%s\n",line);
          else {
            printf("%s\n",prev_line);
            printf("%s\n",line);
          }
        }
        else {
          if (bFirstOnly)
            printf("%5d: %s\n",line_no-1,prev_line);
          else if (bSecondOnly)
            printf("%5d: %s\n",line_no,line);
          else {
            printf("%5d: %s\n",line_no-1,prev_line);
            printf("%5d: %s\n",line_no,line);
          }
        }
      }
    }

    strcpy(prev_line,line);
  }

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
