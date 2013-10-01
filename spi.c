#include <windows.h>
#include <stdio.h>

int main(int argc,char **argv)
{
  RECT rect;

  SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
  printf("%4d %4d\n",rect.right - rect.left,rect.bottom - rect.top);

  return 0;
}
