#ifndef SPCRSA_H
#define SPCRSA_H

#include <stdint.h>

int spc_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

int sat_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

int user_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

int sat_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

int spc_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

int user_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len);

#endif 
