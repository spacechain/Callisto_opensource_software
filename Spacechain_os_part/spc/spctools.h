#ifndef SPCTOOLS_H
#define SPCTOOLS_H

#include <stdint.h>

int array_char_to_hex(uint64_t in_array_len, uint8_t *in_array_char, uint64_t out_array_len, uint8_t *out_array_hex);
int array_hex_to_char(uint64_t in_array_len, uint8_t *in_array_hex, uint64_t out_array_len, uint8_t *out_array_char);

int array_reverse(uint64_t in_array_len, uint8_t *in_array, uint64_t out_array_len, uint8_t *out_array, uint64_t step);

uint8_t read_varint_len(uint8_t *buffer);

int read_varint(uint8_t *buffer, uint8_t varint_len, uint8_t *varint_ptr);

uint8_t gen_varint_len(uint64_t num);

int gen_varint(uint64_t num, uint8_t varint_len, uint8_t *buffer);


#endif
