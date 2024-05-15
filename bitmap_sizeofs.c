#include <windows.h>
#include <stdio.h>

int main(int argc,char **argv)
{
  printf("sizeof (BITMAPFILEHEADER) = %d\n",sizeof (BITMAPFILEHEADER));
  printf("sizeof (BITMAPINFOHEADER) = %d\n",sizeof (BITMAPINFOHEADER));
  printf("sizeof (RGBQUAD) = %d\n",sizeof (RGBQUAD));

  return 0;
}
