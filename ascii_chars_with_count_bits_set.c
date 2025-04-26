#include <stdio.h>
#include <string.h>

static char usage[] = "usage: ascii_chars_with_count_bits_set count\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int count;
  int and_count;
  int bits_set;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&count);

  for (n = 32; n < 128; n++) {
    bits_set = 0;

    for (m = 0; m < 8; m++) {
      if (!m)
        and_count = 1;
      else
        and_count <<= 1;

      if (n & and_count)
        bits_set++;
    }

    if (bits_set == count)
      putchar(n);
  }

  putchar(0x0a);

  return 0;
}
