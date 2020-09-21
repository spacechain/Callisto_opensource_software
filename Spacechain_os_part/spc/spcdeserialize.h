#ifndef SPCDESERIALIZE_H
#define SPCDESERIALIZE_H
#include <stdint.h>




//ALL IN BYTE HEX DATA SIZE
//TX 
#define VERSION_NO_SIZE		4
#define WITNESS_FLAG_SIZE	2
//witness flag 0x00 0x01
#define WITNESS_FLAG		1
#define LOCK_TIME_SIZE		4
//input
#define PREVTXHASHLEN		32
#define PREVTXINDEXLEN		4

//output
#define VALUELEN		8

//patial label
#define PARTIALLABELLEN		5
#define PARTIALVERSIONLEN	1

//nversion
#define NVERSIONLEN		4

//segwitmark
#define SEGMARKLEN		1
#define	SEGFLAGLEN		1


typedef struct
{
	uint8_t prev_tx_hash[32];
	uint8_t prev_tx_index[4];
	uint64_t txin_script_len;
	uint8_t *txin_script;
	uint8_t sequence_no[4];
}__attribute__((packed))SPC_INPUT;


typedef struct
{
	uint8_t value[8];
	uint64_t txout_script_len;
	uint8_t *txout_script;
}__attribute__((packed))SPC_OUTPUT;


typedef struct
{
	uint64_t op_item_len;
	uint8_t *op_item;

}__attribute__((packed))OP_ITEM;


typedef struct
{
	uint64_t value;
	uint16_t version;
	uint64_t op_items_counter;
	uint8_t *op_items_list;

}__attribute__((packed))SPC_WITNESS;


typedef struct 
{
	uint8_t partial_label[PARTIALLABELLEN];
	uint8_t partial_version;
	uint32_t nversion;
	uint8_t segwit_flag;
	uint8_t *inputs_list;
	uint64_t inputs_counter;
	uint8_t *outputs_list;
	uint64_t outputs_counter;
	uint8_t *witnesses_list;
	uint64_t witnesses_counter;
	uint32_t lock_time;	

}__attribute__((packed))SPC_TX;


//check partial



//inputs part
int init_inputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *inputs_counter_ptr, uint8_t **inputs_list_ptr, uint64_t *inputs_len_ptr);
void free_inputs(uint8_t *inputs_list, uint64_t inputs_counter);


//outputs part
int init_outputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *outputs_counter_ptr, uint8_t **outputs_list_ptr, uint64_t *outputs_len_ptr);
void free_outputs(uint8_t *outputs_list, uint64_t outputs_counter);


//witness part
int init_witnesses(uint8_t *buffer, uint64_t buffer_len, uint64_t *witnesses_counter_ptr, uint8_t **witnesses_list_ptr, uint64_t *witnesses_len_ptr);
void free_witnesses(uint8_t *witnesses_list, uint64_t witnesses_counter);
//static 
void free_single_witness(SPC_WITNESS *witness_st_ptr);

//tx
int init_tx(uint8_t *buffer, uint64_t buffer_len, SPC_TX *tx_st_ptr);
void free_tx(SPC_TX *tx_st_ptr);










#endif 
