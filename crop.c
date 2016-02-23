#include <stdio.h>
#include <stdlib.h>
 
#define LEFT      0
#define TOP       1
#define WIDTH     2
#define HEIGHT    3
#define NUM_PARMS 4
 
static char couldnt_open[] = "couldn't open %s\n";
static char fmt_d[] = "%d\n";
static char fmt[] = "%08x\n";
 
int main(int argc,char **argv)
{
  int m;
  int n;
  int parms[NUM_PARMS];
  FILE *fptr;
  int left;
  int top;
  int full_width;
  int full_height;
  int num_cells;
  int *cells;
  int *ipt;
 
  if (argc != 6) {
    printf("usage: crop filename left top width height\n");
    return 1;
  }
 
  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }
 
  for (n = 0; n < NUM_PARMS; n++)
    sscanf(argv[2 + n],"%d",&parms[n]);
 
  fscanf(fptr,"%d",&left);
  fscanf(fptr,"%d",&top);
  fscanf(fptr,"%d",&full_width);
  fscanf(fptr,"%d",&full_height);
 
  if ((parms[LEFT] + parms[WIDTH] > full_width) ||
      (parms[TOP] + parms[HEIGHT] > full_height)) {
    printf("full_width = %d, full_height = %d; out of frame\n",
      full_width,full_height);
    return 3;
  }
 
  num_cells = full_width * full_height;
 
  if ((cells = (int *)malloc(num_cells * sizeof (int))) == NULL) {
    printf("malloc of %d cells failed\n",num_cells);
    return 4;
  }
 
  for (n = 0; n < num_cells; n++) {
    fscanf(fptr,"%x",&cells[n]);
 
    if (feof(fptr)) {
      printf("end of file at cell %d\n",n);
      return 5;
    }
  }
 
  fclose(fptr);
 
  for (n = 0; n < NUM_PARMS; n++)
    printf(fmt_d,parms[n]);
 
  for (m = 0; m < parms[HEIGHT]; m++) {
    ipt = &cells[(parms[TOP] + m) * full_width + parms[LEFT]];
 
    for (n = 0; n < parms[WIDTH]; n++)
      printf(fmt,ipt[n]);
  }
 
  free(cells);
 
  return 0;
}
