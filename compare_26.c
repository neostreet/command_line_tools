#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FILES 26

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: compare_26 (-verbose) f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13\n"
"  f14 f15 f16 f17 f18 f19 f20 f21 f22 f23 f24 f25 f26\n";
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
  bool bVerbose;
  FILE *fptr[NUM_FILES];
  int left[NUM_FILES];
  int top[NUM_FILES];
  int image_width[NUM_FILES];
  int image_height[NUM_FILES];
  int line_len;
  int bytes_to_io;
  int bytes_read;
  int *images[NUM_FILES];
  int matches;

  if ((argc < 27) || (argc > 28)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 26) {
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

  q = 0;

  for (p = 0; p < image_height[0]; p++) {
    for (m = 0; m < image_width[0]; m++) {
      matches = 0;

      for (r = 0; r < NUM_FILES - 1; r++) {
        for (s = r + 1; s < NUM_FILES; s++) {
          if (images[r][q] == images[s][q])
            matches++;
        }
      }

      if (!bVerbose) {
        if (!matches)
          printf("%d %d %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",m,p,
            images[0][q],images[1][q],images[2][q],images[3][q],
            images[4][q],images[5][q],images[6][q],images[7][q],
            images[8][q],images[9][q],images[10][q],images[11][q],
            images[12][q],images[13][q],images[14][q],images[15][q],
            images[16][q],images[17][q],images[18][q],images[19][q],
            images[20][q],images[21][q],images[22][q],images[23][q],
            images[24][q],images[25][q]);
      }
      else
        printf("%d %d %d\n",matches,m,p);

      q++;
    }
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
