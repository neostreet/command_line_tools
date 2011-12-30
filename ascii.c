#include <stdio.h>

int main(int argc,char **argv)
{
  int m;
  int n;
  int chara;

  for (m = 0; m < 16; m++) {
    for (n = 2; n < 8; n++) {
      chara = m + n * 16;
      printf("%c %02x ",chara,chara);
    }

    putchar(0x0a);
  }
}
