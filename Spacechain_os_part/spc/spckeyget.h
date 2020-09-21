#ifndef SPCKEYGET_H
#define SPCKEYGET_H

#include <stdint.h>

#include "bip32.h"
int get_pubkey_from_xpub_base58_double_ckd(uint8_t *xpub_base58, uint32_t xpub_len, uint32_t path1, uint32_t path2, uint8_t *buffer, uint32_t buffer_len);

int get_pubkey_from_xpub_double_ckd(uint8_t *xpub_non_base58, uint32_t xpub_len, uint32_t path1, uint32_t path2, uint8_t *buffer, uint32_t buffer_len);

int get_node_from_xprv_double_ckd(uint8_t *xprv, uint32_t xprv_len, uint32_t path1, uint32_t path2, HDNode *node_ptr);


#endif
