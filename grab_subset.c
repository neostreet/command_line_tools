#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: grab_subset filename left_offset top_offset width height\n";
static char couldnt_open[] = "couldn't open %s\n";
static char fmt1[] = "%d\n";
static char fmt2[] = "%08x\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int left_offset;
  int top_offset;
  int subset_width;
  int subset_height;
  int work;
  int left;
  int top;
  int width;
  int height;
  int num_pixels;
  int *pixels;
  int offset;

  if (argc != 6) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  sscanf(argv[2],"%d",&left_offset);
  sscanf(argv[3],"%d",&top_offset);
  sscanf(argv[4],"%d",&subset_width);
  sscanf(argv[5],"%d",&subset_height);

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (line_no < 4)
      sscanf(line,"%d",&work);
    else
      sscanf(line,"%x",&work);

    switch(line_no) {
      case 0:
        left = work;
        break;
      case 1:
        top = work;
        break;
      case 2:
        width = work;
        break;
      case 3:
        height = work;

        if (left_offset + subset_width > width) {
          printf("left_offset + subset_width > width\n");
          return 3;
        }

        if (top_offset + subset_height > height) {
          printf("top_offset + subset_height > height\n");
          return 4;
        }

        num_pixels = width * height;
        pixels = (int *)malloc(sizeof(int) * num_pixels);

        if (pixels == NULL) {
          printf("couldn't malloc %d ints\n",num_pixels);
          return 5;
        }

        break;
    }

    if (line_no >= 4)
      pixels[line_no - 4] = work;

    line_no++;
  }

  fclose(fptr);

  printf(fmt1,left + left_offset);
  printf(fmt1,top + top_offset);
  printf(fmt1,subset_width);
  printf(fmt1,subset_height);

  for (m = 0; m < subset_height; m++) {
    offset = (top_offset + m) * width + left_offset;

    for (n = 0; n < subset_width; n++)
      printf(fmt2,pixels[offset + n]);
  }

  free(pixels);

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
