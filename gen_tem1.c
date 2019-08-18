#include <stdio.h>

static char usage[] = "usage: gen_tem1 file1 file2\n";
static char couldnt_open[] = "couldn't open %s\n";

static char tempfile[] = "tem.bat";

int main(int argc,char **argv)
{
  FILE *fptr;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(tempfile,"w")) == NULL) {
    printf(couldnt_open,tempfile);
    return 2;
  }

  fprintf(fptr,"@echo off\n\n");
  fprintf(fptr,"ren \"%s\" %s\n",argv[1],argv[2]);

  fclose(fptr);

  return 0;
}
