#include <stdio.h>
#include "bitfuns.h"

#define BIT_STREAM_LEN 8
static unsigned char in_bit_stream[BIT_STREAM_LEN];
static unsigned char out_bit_stream[BIT_STREAM_LEN];

int main(int argc,char **argv)
{
  int n;
  unsigned short in_val;
  unsigned short out_val;

  for (n = 0; n < BIT_STREAM_LEN; n++) {
    in_bit_stream[n] = 'a' + n;
    in_val = get_bits(8,in_bit_stream,n * 8);
    set_bits(8,out_bit_stream,n * 8,in_val);
  }

  for (n = 0; n < BIT_STREAM_LEN * 8; n++) {
    out_val = get_bits(1,in_bit_stream,n);

    if (out_val)
      putchar('1');
    else
      putchar('0');
  }

  putchar(0x0a);

  for (n = 0; n < BIT_STREAM_LEN * 8; n++) {
    out_val = get_bits(1,out_bit_stream,n);

    if (out_val)
      putchar('1');
    else
      putchar('0');
  }

  putchar(0x0a);

  return 0;
}
