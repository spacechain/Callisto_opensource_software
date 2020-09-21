#ifndef SPCSIGN_H
#define SPCSIGN_H


#include "bip32.h"

#include "spcdeserialize.h"


typedef struct
{
	uint64_t sig_len;
	uint8_t *sig;

}__attribute__((packed))SPC_SIG;

typedef struct 
{
	uint64_t sigs_counter;
	uint8_t *sigs;

}__attribute__((packed))SPC_SIGS_LIST;


//static 
int gen_hashprevouts(uint8_t *inputs_list, uint64_t inputs_num, uint8_t *hash_prevouts, uint32_t hashprevouts_len);

//static
int gen_hashsequences(uint8_t *inputs_list, uint64_t inputs_num, uint8_t *hash_sequences, uint32_t hashsequences_len);

//static
int gen_hashoutputs(uint8_t *outputs_list, uint64_t outputs_num, uint8_t *hashoutputs, uint32_t buffer_len);

//static
int gen_scriptcode(SPC_WITNESS *witness_st_ptr, uint8_t *scriptcode_buffer, uint64_t scriptcode_buffer_len);

//static
int get_sigs_list(SPC_WITNESS *witness_st, SPC_SIGS_LIST *sigs_list_st);

//static
void free_sigs_list(SPC_SIGS_LIST *sigs_list_st);

//static
int insert_new_sig_to_list(uint8_t *new_sig, uint64_t new_sig_len, SPC_SIGS_LIST *sigs_list, uint8_t order);

//static
int get_publist_and_paths(SPC_WITNESS *witness_st_ptr, uint8_t *pubs_list, uint8_t *path1s_list, uint8_t *path2s_list);

//static
int get_sigs_num_from_witness(SPC_WITNESS *witness_st);

//static
int update_tx_witness(SPC_TX *tx_st, SPC_WITNESS *new_witness_st, uint64_t witness_index);

//static 
int free_partial_witness(SPC_WITNESS *witness_st);



//api
//sign_transaction()
  //sign_tx();
    //sign_single_input()
      //get_order()
      //get_preimage()
      //get_node_from_xprv_double_ckd()
      //gen_sig()
      //gen_new_witness()
      //update_tx()
//
int get_order_and_paths(uint8_t *xpub, uint32_t xpub_len, SPC_WITNESS *witness_st_ptr, uint8_t *order, uint32_t *path1, uint32_t *path2);
//
int gen_preimage(uint8_t *nversion, uint8_t *inputs_list, uint64_t inputs_num, uint8_t *outputs_list, uint64_t outputs_num, SPC_WITNESS *witness_st_ptr, uint64_t input_no, uint8_t *lock_time, uint8_t *buffer, uint64_t buffer_len);
//1+1+1+1+32+1+1+32
int gen_sig(HDNode *node_ptr, uint8_t *pre_image, uint32_t pre_image_len, uint8_t *sig, uint32_t *sig_len);
//
int gen_new_witness(uint8_t *new_sig, uint64_t new_sig_len, uint8_t order, SPC_WITNESS *old_witness_st, SPC_WITNESS *new_witness_st);
//
int update_tx_after_signed(SPC_TX *tx_st, SPC_WITNESS *new_witness_st, uint64_t witness_index);

//mod function
int sign_single_input(uint8_t *xpub, uint32_t xpub_len, uint8_t *xprv, uint32_t xprv_len, SPC_TX *tx_st, uint64_t witness_index);

//api
int sign_tx(uint8_t *xpub, uint32_t xpub_len, uint8_t *xprv, uint32_t xprv_len, SPC_TX *tx_st_ptr);

#endif
