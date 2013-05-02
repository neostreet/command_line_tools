#define BITS_PER_BYTE      8
#define MAX_BITS          16

unsigned short get_bits(unsigned short num_bits,unsigned char *bit_stream,unsigned int bit_offset);
void set_bits(unsigned short num_bits,unsigned char *bit_stream,unsigned int bit_offset,unsigned short bits);
