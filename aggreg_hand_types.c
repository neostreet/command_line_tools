#include <stdio.h>
#include "str_list.h"

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

#define MAX_HAND_TYPE_LEN 15
static char hand_type[MAX_HAND_TYPE_LEN+1];

static char usage[] = "usage: aggregate_hand_types filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char *plain_hand_types[] = {
  "high card",
  "one pair",
  "two pair",
  "three of a kind",
  "straight",
  "flush",
  "full house",
  "four of a kind",
  "straight flush",
  "royal flush"
};
#define NUM_HAND_TYPES (sizeof plain_hand_types / sizeof (char *))

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int get_hand_type_and_delta(
  char *line,
  int line_len,
  char *hand_type,
  int max_hand_type_len,
  int *delta_ptr);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int retval;
  struct info_list hand_types;
  struct info_list_elem *work_elem;
  int ix;
  int delta;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;
  hand_types.num_elems = 0;

  for (n = 0; n < NUM_HAND_TYPES; n++)
    add_info_list_elem(&hand_types,plain_hand_types[n],0,0,0,true);

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    retval = get_hand_type_and_delta(line,line_len,hand_type,MAX_HAND_TYPE_LEN,
      &delta);

    if (retval) {
      printf("get_hand_type() failed on line %d: %d\n",line_no,retval);
      return 3;
    }

    if (member_of_info_list(&hand_types,hand_type,&ix)) {
      if (get_info_list_elem(&hand_types,ix,&work_elem)) {
        work_elem->int1++;

        if (delta > 0)
          work_elem->int2 += delta;
        else
          work_elem->int3 += delta;
      }
    }
    else {
      if (delta > 0)
        add_info_list_elem(&hand_types,hand_type,1,delta,0,true);
      else
        add_info_list_elem(&hand_types,hand_type,1,0,delta,true);
    }
  }

  fclose(fptr);

  work_elem = hand_types.first_elem;

  for (n = 0; n < hand_types.num_elems; n++) {
    printf("%10d %10d %10d %5d %s\n",
      work_elem->int2 + work_elem->int3,
      work_elem->int2,
      work_elem->int3,
      work_elem->int1,
      work_elem->str);
    work_elem = work_elem->next_elem;
  }

  free_info_list(&hand_types);

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

static int get_hand_type_and_delta(
  char *line,
  int line_len,
  char *hand_type,
  int max_hand_type_len,
  int *delta_ptr)
{
  int m;
  int n;
  char delta_str[11];

  for (m = 0, n = 17; ((n < 17 + max_hand_type_len) && (n < line_len - 1)); n++) {
    if ((line[n] == 'c') && (line[n+1] == ':'))
      break;

    hand_type[m++] = line[n];
  }

  if (!m)
    return 1;

  if (hand_type[m - 1] == ' ')
    hand_type[m - 1] = 0;
  else
    hand_type[m] = 0;

  for (m = 6; m < 16; m++) {
    if (line[m] != ' ')
      break;
  }

  if (m == 16)
    return 2;

  for (n = 0; m < 16; m++)
    delta_str[n++] = line[m];

  delta_str[n] = 0;

  sscanf(delta_str,"%d",delta_ptr);

  return 0;
}
