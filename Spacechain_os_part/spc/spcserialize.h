#ifndef SPCSERIALIZE_H
#define SPCSERIALIZE_H

#include <stdint.h>
#include "spcdeserialize.h"



///
int get_inputs_hex_len(uint8_t *inputs_list, uint64_t inputs_counter, uint64_t *inputs_hex_len);

///
int get_outputs_hex_len(uint8_t *outputs_list, uint64_t outputs_counter, uint64_t *outputs_hex_len);

///
int get_witnesses_hex_len(uint8_t *witnesses_list, uint64_t witnesses_counter, uint64_t *witnesses_hex_len);

//
int get_tx_hex_len(SPC_TX *tx_st_ptr, uint64_t *tx_hex_len);


///
int serialize_inputs(uint8_t *inputs_list, uint64_t inputs_counter, uint8_t *buffer, uint64_t *inputs_hex_len);

///
int serialize_outputs(uint8_t *outputs_list, uint64_t outputs_counter, uint8_t *buffer, uint64_t *outputs_hex_len);

///
int serialize_witnesses(uint8_t *witnesses_list, uint64_t witnesses_counter, uint8_t *buffer, uint64_t *witnesses_hex_len);

//
int serialize_tx(SPC_TX *tx_st_ptr, uint8_t *buffer, uint64_t *tx_hex_len);


#endif 
