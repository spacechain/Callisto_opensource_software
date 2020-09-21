#ifndef SPCKEYSTORE_H
#define SPCKEYSTORE_H

#include <stdint.h>

#include "spcconfig.h"


typedef struct
{
	uint32_t id;
	uint32_t key_pairs_num;
}__attribute__((packed))SPC_KEYSTORE;



typedef struct
{
	uint8_t xpub[113];
	uint8_t xprv[113];
}__attribute__((packed))SPC_KEY_PAIR;



//API
int gen_new_keystore(uint32_t id);

//API
int get_key_pair(uint32_t id, uint16_t index, SPC_KEY_PAIR *key_pair_st_ptr);

//API
int save_key_pair(uint32_t id, SPC_KEY_PAIR *key_pair_st_ptr, uint16_t *index);



#endif 
