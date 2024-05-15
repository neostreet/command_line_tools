#include <stdio.h>

static char usage[] = "usage: swap_nibbles char\n";

int swap_nibbles(int chara);

int main(int argc,char **argv)
{
  int chara;
  int swapped;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%x",&chara);
  swapped = swap_nibbles(chara);

  printf("chara = %x\n",chara);
  printf("swapped = %x\n",swapped);

  return 0;
}

int swap_nibbles(int chara)
{
  int upper_nibble;
  int lower_nibble;
  int swapped;

  upper_nibble = (chara & 0xf0) >> 4;
  lower_nibble = (chara & 0x0f);

  swapped = (lower_nibble << 4) | upper_nibble;

  return swapped;
}
