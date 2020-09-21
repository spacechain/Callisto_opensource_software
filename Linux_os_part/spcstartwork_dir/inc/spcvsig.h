#ifndef SPCVSIG_H
#define SPCVSIG_H

#include <stdint.h>

int spc_verify_sig_with_md5(uint8_t *sig, uint32_t sig_len, uint8_t *md5, uint32_t md5_len);

#endif 
