#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int left;
  int top;
  int width;
  int height;
  int num_cells;
  int *cells;
  int color;
  FILE *fptr;

  if (argc != 3) {
    printf("usage: find_color color filename\n");
    return 1;
  }

  sscanf(argv[1],"%x",&color);

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf("couldn't open %s\n",argv[2]);
    return 2;
  }

  fscanf(fptr,"%d",&left);
  fscanf(fptr,"%d",&top);
  fscanf(fptr,"%d",&width);
  fscanf(fptr,"%d",&height);

  num_cells = width * height;
 
  if ((cells = (int *)malloc(num_cells * sizeof (int))) == NULL) {
    printf("malloc of %d cells failed\n",num_cells);
    return 3;
  }
 
  for (n = 0; n < num_cells; n++) {
    fscanf(fptr,"%x",&cells[n]);
 
    if (feof(fptr)) {
      printf("end of file at cell %d\n",n);
      return 4;
    }
  }

  p = 0;

  for (m = 0; m < height; m++) {
    for (n = 0; n < width; n++) {
      if (cells[p] == color)
        printf("%d %d\n",n,m);

      p++;
    }
  }

  free(cells);
  fclose(fptr);

  return 0;
}
