#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FILES 26

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: compare_26_2 (-debug) (-verbose) f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13\n"
"  f14 f15 f16 f17 f18 f19 f20 f21 f22 f23 f24 f25 f26\n"
"  left2_1 top2_1 left2_2 top2_2";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int q;
  int r;
  int s;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  FILE *fptr[NUM_FILES];
  int left[NUM_FILES];
  int top[NUM_FILES];
  int left2[2];
  int top2[2];
  int offset[2];
  int image_width[NUM_FILES];
  int image_height[NUM_FILES];
  int line_len;
  int bytes_to_io;
  int bytes_read;
  int *images[NUM_FILES];
  int matches;

  if ((argc < 31) || (argc > 33)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 30) {
    printf(usage);
    return 2;
  }

  for (n = 0; n < NUM_FILES; n++) {
    if ((fptr[n] = fopen(argv[curr_arg+n],"r")) == NULL) {
      printf(couldnt_open,argv[curr_arg+n]);
      return 3;
    }

    fscanf(fptr[n],"%d\n",&left[n]);
    fscanf(fptr[n],"%d\n",&top[n]);
    fscanf(fptr[n],"%d\n",&image_width[n]);
    fscanf(fptr[n],"%d\n",&image_height[n]);

    if ((n) && ((image_width[n] != image_width[0]) || (image_height[n] != image_height[0]))) {
      printf("different widths and/or different heights\n");
      return 4;
    }
  }

  sscanf(argv[curr_arg+26],"%d",&left2[0]);
  sscanf(argv[curr_arg+27],"%d",&top2[0]);
  sscanf(argv[curr_arg+28],"%d",&left2[1]);
  sscanf(argv[curr_arg+29],"%d",&top2[1]);

  bytes_to_io = sizeof (int) * image_width[0] * image_height[0];

  for (n = 0; n < NUM_FILES; n++) {
    if ((images[n] = (int *)malloc(bytes_to_io)) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_io);
      return 5;
    }
  }

  for (n = 0; n < NUM_FILES; n++) {
    q = 0;

    for (p = 0; p < image_height[0]; p++) {
      for (m = 0; m < image_width[0]; m++) {
        GetLine(fptr[n],line,&line_len,MAX_LINE_LEN);

        if (feof(fptr[n]))
          break;

        sscanf(line,"%x",&images[n][q++]);
      }
    }
  }

  for (p = 0; p < 2; p++)
    offset[p] = top2[p] * image_width[0] + left2[p];

  matches = 0;

  for (r = 0; r < NUM_FILES - 1; r++) {
    for (s = r + 1; s < NUM_FILES; s++) {
      if ((images[r][offset[0]] == images[s][offset[0]]) &&
          (images[r][offset[1]] == images[s][offset[1]])) {
        matches++;

        if (bDebug)
          printf("match: %d %d %x %x\n",r,s,images[r][offset[0]],images[r][offset[1]]);
      }
    }
  }

  if (bVerbose) {
    for (r = 0; r < NUM_FILES; r++)
      printf("%d %x %x\n",r,images[r][offset[0]],images[r][offset[1]]);
  }

  for (n = 0; n < NUM_FILES; n++) {
    free(images[n]);
    fclose(fptr[n]);
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
