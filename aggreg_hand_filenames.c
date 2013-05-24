#include <stdio.h>
#include "str_list.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_HAND_FILENAME_LEN 128
static char hand_filename[MAX_HAND_FILENAME_LEN+1];

static char usage[] = "usage: aggregate_hand_filenames offset filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_hand_filename(
  char *line,
  int line_len,
  int offset,
  char *hand_filename,
  int max_hand_filename_len
);

int main(int argc,char **argv)
{
  int n;
  int offset;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  struct info_list hand_filenames;
  struct info_list_elem *work_elem;
  int ix;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&offset);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

  line_no = 0;
  hand_filenames.num_elems = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = get_hand_filename(line,line_len,offset,hand_filename,MAX_HAND_FILENAME_LEN);

    if (retval) {
      printf("get_hand_filename() failed on line %d: %d\n",line_no,retval);
      return 3;
    }

    if (member_of_info_list(&hand_filenames,hand_filename,&ix)) {
      if (get_info_list_elem(&hand_filenames,ix,&work_elem))
        work_elem->int1++;
    }
    else
      add_info_list_elem(&hand_filenames,hand_filename,1,0,0,true);
  }

  fclose(fptr);

  work_elem = hand_filenames.first_elem;

  for (n = 0; n < hand_filenames.num_elems; n++) {
    printf("%3d %s\n",
      work_elem->int1,
      work_elem->str);
    work_elem = work_elem->next_elem;
  }

  free_info_list(&hand_filenames);

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

static int get_hand_filename(
  char *line,
  int line_len,
  int offset,
  char *hand_filename,
  int max_hand_filename_len
)
{
  int m;
  int n;

  for (m = 0, n = offset; ((n < offset + max_hand_filename_len) && (n < line_len - 1)); n++) {
    if (line[n] == ' ')
      break;

    hand_filename[m++] = line[n];
  }

  if (!m)
    return 1;

  hand_filename[m] = 0;

  return 0;
}
