#include "bitfuns.h"

static unsigned short and_vals[] = {
  0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

unsigned short get_bits(
unsigned short num_bits,
unsigned char *bit_stream,
unsigned int bit_offset
)
{
  unsigned short n;
  unsigned int byte_offset;
  unsigned short bit_remainder;
  unsigned short bit_depth;
  unsigned short num_bytes;
  unsigned short bits_in_last_byte;
  unsigned short work;
  unsigned short pad_bits;
  unsigned short curr_pad_bits;
  unsigned short retval;
  short bLastChar;

  byte_offset = bit_offset / BITS_PER_BYTE;
  bit_remainder = (unsigned short)(bit_offset - byte_offset * BITS_PER_BYTE);

  bit_depth = bit_remainder + num_bits;
  num_bytes = (bit_depth + BITS_PER_BYTE - 1) / BITS_PER_BYTE;

  if (num_bytes > 1) {
    bits_in_last_byte = bit_depth % BITS_PER_BYTE;

    if (!bits_in_last_byte)
      bits_in_last_byte = BITS_PER_BYTE;
  }

  retval = 0;

  for (n = 0; n < num_bytes; n++) {
    work = bit_stream[byte_offset + n];
    bLastChar = 0;

    switch (n) {
      case 0:
        /* get rid of extraneous bits */
        if (bit_remainder)
          work &= and_vals[bit_remainder - 1];

        if (num_bytes == 1) {
          if (bit_depth != BITS_PER_BYTE)
            work >>= (BITS_PER_BYTE - bit_depth);
        }
        else {
          pad_bits = MAX_BITS - num_bits;
          curr_pad_bits = BITS_PER_BYTE + bit_remainder;

          if (curr_pad_bits > pad_bits)
            work <<= (curr_pad_bits - pad_bits);
        }

        break;
      case 1:
        if (num_bytes == 2)
          bLastChar = 1;
        else  /* num_bytes == 3 */
          work <<= bits_in_last_byte;

        break;
      case 2:
        bLastChar = 1;

        break;
    }

    if (bLastChar) {
      if (bits_in_last_byte != BITS_PER_BYTE)
        work >>= (BITS_PER_BYTE - bits_in_last_byte);
    }

    retval |= work;
  }

  return retval;
}

void set_bits(
unsigned short num_bits,
unsigned char *bit_stream,
unsigned int bit_offset,
unsigned short bits)
{
  unsigned short n;
  unsigned int byte_offset;
  unsigned short bit_remainder;
  unsigned short bit_depth;
  unsigned short num_bytes;
  unsigned short bits_in_last_byte;
  unsigned short work;
  unsigned short shift;
  unsigned short and_value;
  unsigned short or_value;
  short bLastChar;

  /* clear the unused high bits as a precaution */
  if (num_bits < MAX_BITS) {
    and_value = 0;

    for (n = 0; n < num_bits; n++) {
      and_value <<= 1;
      and_value |= 1;
    }

    bits &= and_value;
  }

  byte_offset = bit_offset / BITS_PER_BYTE;
  bit_remainder = (unsigned short)(bit_offset - byte_offset * BITS_PER_BYTE);

  bit_depth = bit_remainder + num_bits;
  num_bytes = (bit_depth + BITS_PER_BYTE - 1) / BITS_PER_BYTE;

  if (num_bytes > 1) {
    bits_in_last_byte = bit_depth % BITS_PER_BYTE;

    if (!bits_in_last_byte)
      bits_in_last_byte = BITS_PER_BYTE;
  }

  for (n = 0; n < num_bytes; n++) {
    work = bit_stream[byte_offset + n];
    bLastChar = 0;

    switch (n) {
      case 0:
        if (num_bytes == 1) {
          if (num_bits == BITS_PER_BYTE)
            work = bits;
          else {
            shift = BITS_PER_BYTE - bit_depth;
            and_value = and_vals[BITS_PER_BYTE - (num_bits + 1)];
            or_value = bits;

            if (shift) {
              and_value <<= shift;
              or_value <<= shift;
            }

            work &= ~and_value;
            work |= or_value;
          }
        }
        else {
          if (!bit_remainder)
            work = 0;
          else
            work &= ~and_vals[bit_remainder - 1];

          or_value = bits;
          shift = num_bits - (BITS_PER_BYTE - bit_remainder);
          or_value >>= shift;

          work |= or_value;
        }

        break;
      case 1:
        if (num_bytes == 2)
          bLastChar = 1;
        else
          work = bits >> bits_in_last_byte;

        break;
      case 2:
        bLastChar = 1;

        break;
    }

    if (bLastChar) {
      if (bits_in_last_byte == BITS_PER_BYTE)
        work = bits;
      else {
        work &= and_vals[bits_in_last_byte - 1];
        or_value = bits &
          and_vals[BITS_PER_BYTE - (bits_in_last_byte + 1)];
        shift = BITS_PER_BYTE - bits_in_last_byte;
        or_value <<= shift;
        work |= or_value;
      }
    }

    bit_stream[byte_offset + n] = (unsigned char)work;
  }
}
