#include <stdio.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: append filename filename (filename ...)\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int num_files;
  FILE *fptr[13];
  int left[13];
  int top[13];
  int image_width[13];
  int image_height[13];
  int line_len;

  if ((argc < 3) || (argc > 14)) {
    printf(usage);
    return 1;
  }

  num_files = argc - 1;

  for (n = 0; n < num_files; n++) {
    if ((fptr[n] = fopen(argv[1+n],"r")) == NULL) {
      printf(couldnt_open,argv[1+n]);
      return 2;
    }

    fscanf(fptr[n],"%d\n",&left[n]);
    fscanf(fptr[n],"%d\n",&top[n]);
    fscanf(fptr[n],"%d\n",&image_width[n]);
    fscanf(fptr[n],"%d\n",&image_height[n]);

    if ((n) && ((image_width[n] != image_width[0]) || (image_height[n] != image_height[0]))) {
      printf("different widths and/or different heights\n");
      return 3;
    }
  }

  printf("%d\n",left[0]);
  printf("%d\n",top[0]);
  printf("%d\n",num_files * image_width[0]);
  printf("%d\n",image_height[0]);

  for (p = 0; p < image_height[0]; p++) {
    for (n = 0; n < num_files; n++) {
      for (m = 0; m < image_width[0]; m++) {
        GetLine(fptr[n],line,&line_len,MAX_LINE_LEN);

        if (feof(fptr[n]))
          break;

        printf("%s\n",line);
      }
    }
  }

  for (n = 0; n < num_files; n++)
    fclose(fptr[n]);

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
