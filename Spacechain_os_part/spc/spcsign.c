#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "spctools.h"
#include "spcsign.h"
#include "spcdeserialize.h"
#include "spcsha256d.h"
#include "spcscript.h"
#include "spckeyget.h"


int gen_hashprevouts(uint8_t *inputs_list, uint64_t inputs_num, uint8_t *hashprevouts, uint32_t buffer_len)
{
	if((NULL == inputs_list) || (inputs_num == 0) || (NULL == hashprevouts) || (buffer_len != 32))
	{
		return -1;
	}

	uint8_t *hash_in = NULL;
	hash_in = malloc(inputs_num * (32+4));
	if(NULL == hash_in)
	{
		return -1;
	}

	uint64_t i = 0;
	SPC_INPUT input_st;
	for(i = 0; i < inputs_num; i++)
	{
		memset(&input_st, '\0', sizeof(input_st));
		memcpy(&input_st, inputs_list + i*sizeof(SPC_INPUT), sizeof(SPC_INPUT));
		memcpy(hash_in + i*(32+4), input_st.prev_tx_hash, 32);
		memcpy(hash_in + i*(32+4) + 32, input_st.prev_tx_index, 4);
	}	

	if((inputs_num * 36) > 0xffffffff)
	{
		printf("HASH IN DATA OVERSIZE CAN NOT BE HASHED\n");
		free(hash_in);
		return -1;
	}

	uint32_t length = inputs_num * (32+4); 
	sha256d(hash_in, length, hashprevouts, 32);
	free(hash_in);

	return 0;
}


int gen_hashsequences(uint8_t *inputs_list, uint64_t inputs_num, uint8_t *hashsequences, uint32_t buffer_len)
{
	if((NULL == inputs_list) || (inputs_num == 0) || (NULL == hashsequences) || (buffer_len != 32))
	{
		return -1;
	}

	uint8_t *hash_in = NULL;
	hash_in = malloc(inputs_num * (4));
	if(NULL == hash_in)
	{
		return -1;
	}

	uint64_t i = 0;
	SPC_INPUT input_st;
	for(i = 0; i < inputs_num; i++)
	{
		memset(&input_st, '\0', sizeof(input_st));
		memcpy(&input_st, inputs_list + i*sizeof(SPC_INPUT), sizeof(SPC_INPUT));
		memcpy(hash_in + i*(4), input_st.sequence_no, 4);
	}	

	if((inputs_num * 4) > 0xffffffff)
	{
		printf("HASH IN DATA OVERSIZE CAN NOT BE HASHED\n");
		free(hash_in);
		return -1;
	}

	uint32_t length = inputs_num * (4); 
	sha256d(hash_in, length, hashsequences, 32);
	free(hash_in);

	return 0;
}


int gen_hashoutputs(uint8_t *outputs_list, uint64_t outputs_num, uint8_t *hashoutputs, uint32_t buffer_len)
{
	if((NULL == outputs_list) || (outputs_num == 0) || (NULL == hashoutputs) || (buffer_len != 32))
	{
		return -1;
	}

	uint8_t *hash_in = NULL;
	uint64_t counter = 0;
	uint64_t i = 0;
	SPC_OUTPUT output_st;
	uint8_t varint_len = 0;
	uint8_t varint1[1] = {'\0'};
	uint8_t varint3[3] = {'\0'};
	uint8_t varint5[5] = {'\0'};
	uint8_t varint9[9] = {'\0'};
	int ret = -1;
	uint64_t sum = 0;
	for(i = 0; i < outputs_num; i++)
	{
		memset(&output_st, '\0', sizeof(output_st));
		memcpy(&output_st, outputs_list + i*sizeof(SPC_OUTPUT), sizeof(SPC_OUTPUT));
		//add 8
		sum += 8;
		//get varint_len
		varint_len = gen_varint_len(output_st.txout_script_len);
		//add varint_len
		sum+= varint_len;
		//add script len
		sum+= output_st.txout_script_len;
	
	}

	hash_in = malloc(sum);
	if(NULL == hash_in)
	{
		return -1;
	}
	memset(hash_in, '\0', sum);

	counter = 0;
	for(i = 0; i < outputs_num; i++)
	{
		printf("DEBUG:GEN_HASHOUTPUTS():outputs:%ld\n", i);
		memset(varint1, '\0', 1);
		memset(varint3, '\0', 3);
		memset(varint5, '\0', 5);
		memset(varint9, '\0', 9);
		
		memset(&output_st, '\0', sizeof(output_st));
		memcpy(&output_st, outputs_list + i*sizeof(SPC_OUTPUT), sizeof(SPC_OUTPUT));
		
		//get txout value
		memcpy(hash_in + counter, output_st.value, 8);
		counter += 8;
		printf("DEBUG:GEN_HASHOUTPUTS:AFTER1\n");

		//get txout script varint
		varint_len = gen_varint_len(output_st.txout_script_len);
		printf("DEBUG:GET VARINT LEN:%d\n", varint_len);
		if(1 == varint_len)
		{
			ret = gen_varint(output_st.txout_script_len, varint_len, varint1);
			if(ret == -1)
			{
				free(hash_in);
				return -1;
			}
			memcpy(hash_in + counter, varint1, 1);
			counter += 1;
			printf("DEBUG:GEN_HASHOUTPUTS:AFTER2\n");
			
		}
		else if(3 == varint_len)
		{
			ret = gen_varint(output_st.txout_script_len, varint_len, varint3);
			if(ret == -1)
			{
				free(hash_in);
				return -1;
			}
			memcpy(hash_in + counter, varint3, 3);
			counter += 3;
			printf("DEBUG:GEN_HASHOUTPUTS:AFTER2\n");
			

		}
		else if(5 == varint_len)
		{
			ret = gen_varint(output_st.txout_script_len, varint_len, varint5);
			if(ret == -1)
			{
				free(hash_in);
				return -1;
			}
			memcpy(hash_in + counter, varint5, 5);
			counter += 5;
			printf("DEBUG:GEN_HASHOUTPUTS:AFTER2\n");
			
		}
		else if(9 == varint_len)
		{
			ret = gen_varint(output_st.txout_script_len, varint_len, varint9);
			if(ret == -1)
			{
				free(hash_in);
				return -1;
			}
			memcpy(hash_in + counter, varint9, 9);
			counter += 9;
			printf("DEBUG:GEN_HASHOUTPUTS:AFTER2\n");
			
		}
		else
		{
			free(hash_in);
			return -1;
		}

		memcpy(hash_in + counter, output_st.txout_script, output_st.txout_script_len);
		counter += output_st.txout_script_len;
	}	

	if(sum != counter)
	{
		printf("DEBUG:SUM COUNTER DO NOT MATCH\n");
		free(hash_in);
		return -1;
	}

	if(counter > 0xffffffff)
	{
		printf("HASH IN DATA OVERSIZE CAN NOT BE HASHED\n");
		free(hash_in);
		return -1;
	}

	uint32_t length = counter; 
	sha256d(hash_in, length, hashoutputs, 32);
	free(hash_in);

	return 0;
}


int gen_scriptcode(SPC_WITNESS *witness_st_ptr, uint8_t *scriptcode_buffer, uint64_t scriptcode_buffer_len)
{
	if((NULL == witness_st_ptr) || (NULL == scriptcode_buffer) || (106 != scriptcode_buffer_len))
	{
		return -1;
	}

	uint8_t scriptcode[106] = {'\0'};
	uint64_t counter = 0;
	//add script code head to buffer
	scriptcode[0] = 0x69;
	scriptcode[1] = 0x52;
	counter += 2;

	//parse last item to script struct
	if(witness_st_ptr->op_items_counter != 5)
	{
		printf("gen_scriptcode():%ld\n", witness_st_ptr->op_items_counter);
		return -1;
	}

	int ret = -1;
	OP_ITEM op_item_st;	
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	memcpy(&op_item_st, (witness_st_ptr->op_items_list)+4*sizeof(OP_ITEM), sizeof(OP_ITEM));
	SPC_SCRIPT script_st;
	memset(&script_st, '\0', sizeof(SPC_SCRIPT));
	ret = parse_script(op_item_st.op_item, op_item_st.op_item_len, &script_st);
	if(ret != 0)
	{
		return -1;
	}
	if(script_st.items_num != 6)
	{
		free_script(&script_st);
		return -1;
	}

	uint8_t x_pubkey[78] = {'\0'};
	uint8_t pubkey[33] = {'\0'};
	uint16_t path_1 = 0;
	uint16_t path_2 = 0;
	uint32_t path_1_32 = 0;
	uint32_t path_2_32 = 0;

	int i = 0;
	SCRIPT_ITEM script_item_st;
	for(i = 0; i < 3; i ++)
	{
		//get x_pubkey
		memset(&script_item_st, '\0', sizeof(SCRIPT_ITEM));
		memcpy(&script_item_st, (script_st.items_list)+(i+1)*sizeof(SCRIPT_ITEM), sizeof(SCRIPT_ITEM));
		memcpy(x_pubkey, (script_item_st.item)+1, 78);
		//get path_1
		memcpy(&path_1, (script_item_st.item)+1+78, sizeof(uint16_t));
		path_1_32 = path_1;
		printf("gen_scriptcode():path1:%d\n", path_1_32); 
		//get path_2
		memcpy(&path_2, (script_item_st.item)+1+78+2, sizeof(uint16_t));
		path_2_32 = path_2;
		printf("gen_scriptcode():path2:%d\n", path_2_32); 
		
		//get pubkey from xpubkey		
		ret = get_pubkey_from_xpub_double_ckd(x_pubkey, 78, path_1_32, path_2_32, pubkey, 33);
		if(ret != 0)
		{
			free_script(&script_st);
			return -1;
		}

		scriptcode[counter] = 0x21;
		counter += 1;
		memcpy(scriptcode + counter, pubkey, 33);
		counter += 33;

	}

	free_script(&script_st);
	scriptcode[counter] = 0x53;
	counter += 1;
	scriptcode[counter] = 0xae;
	counter += 1;

	if(counter != 106)
	{
		return -1;
	}

	memcpy(scriptcode_buffer, scriptcode, 106);

	return 0;
}

int gen_preimage(uint8_t *nversion, uint8_t *inputs_list, uint64_t inputs_num, uint8_t *outputs_list, uint64_t outputs_num, SPC_WITNESS *witness_st_ptr, uint64_t input_no, uint8_t *lock_time,  uint8_t *buffer, uint64_t buffer_len)
{
	if((NULL == nversion) || (NULL == inputs_list) || (0 == inputs_num) || (NULL == outputs_list) || (0 == outputs_num) || (NULL == witness_st_ptr) || (input_no >= inputs_num) || (NULL == lock_time) || (NULL == buffer) || (buffer_len < (4 + 32*3 + 32 + 4 + 106 + 8 + 4 + 4 + 4)))
	{
		return -1;
	} 

	//printf("gen_preimage():inputs check done\n");

	//nversion 		4
	//hashprevouts		32
	//hashsequence		32
	//outpoint		32 + 4
	//sriptscode		106
	//amount		8
	//nsequence		4
	//hashoutputs		32
	//nlocktime		4
	//nhashtype		4
	uint8_t pre_image[4+32+32+32+4+106+8+4+32+4+4] = {'\0'};
	uint64_t counter = 0;
	int ret = -1;
	//
	memcpy(pre_image + counter, nversion, 4);
	counter += 4;
	//
	ret = gen_hashprevouts(inputs_list, inputs_num, pre_image + counter, 32);
	if(ret != 0)
	{
		printf("gen_hashprevouts()\n");	
		return -1;
	}	
	counter += 32;
	//
	ret = gen_hashsequences(inputs_list, inputs_num, pre_image + counter, 32);
	if(ret != 0)
	{
		printf("gen_hashsequences()\n");
		return -1;
	}
	counter += 32;
	//
	SPC_INPUT input_st;
	memcpy(&input_st, inputs_list+input_no*sizeof(SPC_INPUT), sizeof(SPC_INPUT));
	memcpy(pre_image+counter, input_st.prev_tx_hash, 32);
	counter += 32;
	memcpy(pre_image+counter, input_st.prev_tx_index, 4);
	counter += 4;	
	//
	printf("GEN_PREIMAGE():BEFORE GEN_SCRIPTCODE:\n");
	ret = gen_scriptcode(witness_st_ptr, pre_image+counter, 106);
	if(ret != 0)
	{
		printf("gen_scriptcode()\n");
		return -1;
	}
	printf("GEN_PREIMAGE():AFTER GEN_SCRIPTCODE:\n");
	counter += 106;
	//
	memcpy(pre_image+counter, &(witness_st_ptr->value), sizeof(uint64_t));
	counter += sizeof(uint64_t);
	
	printf("GEN_PREIMAGE():AFTER 1\n");
	//
	memcpy(pre_image+counter, input_st.sequence_no, 4);
	counter += 4;
	printf("GEN_PREIMAGE():AFTER 2\n");
	//
	printf("DEBUG:BEFORE GEN HASHOUTPUS:\n");
	printf("outputs_num:%ld\n", outputs_num); 
	ret = gen_hashoutputs(outputs_list, outputs_num, pre_image+counter, 32);
	if(ret != 0)
	{
		printf("gen_hashoutputs()\n");
		return -1;
	}
	counter += 32;
	printf("GEN_PREIMAGE():AFTER 3\n");
	//
	memcpy(pre_image+counter, lock_time, 4);
	counter += 4;
	printf("GEN_PREIMAGE():AFTER 4\n");
	//
	uint32_t hashtype = 1;
	memcpy(pre_image+counter, &hashtype, 4);
	counter += 4;
	printf("GEN_PREIMAGE():5\n");

	//printf("gen_preimage():counter:%ld\n", counter);
	if(counter != 262)
	{
		return -1;
	}

	//copy to buffer
	memcpy(buffer, pre_image, 262);
	return 0;
}


int get_publist_and_paths(SPC_WITNESS *witness_st_ptr, uint8_t *pubs_list, uint8_t *path1s_list, uint8_t *path2s_list)
{
	if((NULL == witness_st_ptr) || (NULL == pubs_list) || (NULL == path1s_list) || (NULL == path2s_list))
	{
		return -1;
	}

	//parse last item to script struct
	if(witness_st_ptr->op_items_counter != 5)
	{
		return -1;
	}

	int ret = -1;
	OP_ITEM op_item_st;	
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	memcpy(&op_item_st, (witness_st_ptr->op_items_list)+4*sizeof(OP_ITEM), sizeof(OP_ITEM));
	SPC_SCRIPT script_st;
	memset(&script_st, '\0', sizeof(SPC_SCRIPT));
	ret = parse_script(op_item_st.op_item, op_item_st.op_item_len, &script_st);
	if(ret != 0)
	{
		return -1;
	}
	if(script_st.items_num != 6)
	{
		free_script(&script_st);
		return -1;
	}

	uint8_t x_pubkey[78] = {'\0'};
	uint8_t pubkey[33] = {'\0'};
	uint16_t path_1 = 0;
	uint16_t path_2 = 0;
	uint32_t path_1_32 = 0;
	uint32_t path_2_32 = 0;

	int i = 0;
	SCRIPT_ITEM script_item_st;
	for(i = 0; i < 3; i ++)
	{
		//get x_pubkey
		memset(&script_item_st, '\0', sizeof(SCRIPT_ITEM));
		memcpy(&script_item_st, (script_st.items_list)+(i+1)*sizeof(SCRIPT_ITEM), sizeof(SCRIPT_ITEM));
		memcpy(x_pubkey, (script_item_st.item)+1, 78);
		//get path_1
		memcpy(&path_1, (script_item_st.item)+1+78, sizeof(uint16_t));
		path_1_32 = path_1; 
		memcpy(path1s_list + i*sizeof(uint32_t), (uint8_t *)(&path_1_32), sizeof(uint32_t));
		//get path_2
		memcpy(&path_2, (script_item_st.item)+1+78+2, sizeof(uint16_t));
		path_2_32 = path_2;
		memcpy(path2s_list + i*sizeof(uint32_t), (uint8_t *)(&path_2_32), sizeof(uint32_t));
		
		//get pubkey from xpubkey		
		ret = get_pubkey_from_xpub_double_ckd(x_pubkey, 78, path_1_32, path_2_32, pubkey, 33);
		if(ret != 0)
		{
			free_script(&script_st);
			return -1;
		}

		memcpy(pubs_list + i*33, pubkey, 33);

	}

	free_script(&script_st);

	return 0;
}


int get_order_and_paths(uint8_t *xpub, uint32_t xpub_len, SPC_WITNESS *witness_st_ptr, uint8_t *order, uint32_t *path1, uint32_t *path2)
{
	//input check
	if((NULL == xpub) || (xpub_len > 113) || (NULL == witness_st_ptr) || (NULL == order) || (NULL == path1) || (NULL == path2))
	{
		printf("get_order_and_paths():wrong inputs.\n");
		return -1;
	}

	//get pubs list path1s_list and path2s_list
	int ret = -1;
	uint8_t pubs_list[3*33] = {'\0'};
	uint8_t path1s_list[3*4] = {'\0'};
	uint8_t path2s_list[3*4] = {'\0'};
	
	ret = get_publist_and_paths(witness_st_ptr, pubs_list, path1s_list, path2s_list);
	if(ret == -1)
	{
		return -1;
	}

	//get every pub followed every path and check it
	int i = 0;
	uint8_t pub_get[33] = {'\0'};
	uint32_t path1_32;
	uint32_t path2_32;
	uint8_t order_flag = 3;
	for(i = 0; i < 3; i ++)
	{
		memset(pub_get, '\0', 33);
		memcpy(&path1_32, path1s_list+(i*4), sizeof(uint32_t));
		memcpy(&path2_32, path2s_list+(i*4), sizeof(uint32_t));
		ret = get_pubkey_from_xpub_base58_double_ckd(xpub, xpub_len, path1_32, path2_32, pub_get, 33);
		if(ret == -1)
		{
			return -1;
		}
		ret = -1;	
		ret = memcmp(pub_get, pubs_list+(i*33), 33);
		if(ret == 0)
		{
			order_flag = i;
			break;
		}

	}	

	if(order_flag > 2)
	{
		return -1;
	}

	memcpy(path1, &path1_32, sizeof(uint32_t));
	memcpy(path2, &path2_32, sizeof(uint32_t));
	memcpy(order, &order_flag, sizeof(uint8_t));

	return 0;
}





int gen_sig(HDNode *node_ptr, uint8_t *pre_image, uint32_t pre_image_len, uint8_t *sig, uint32_t *sig_len)
{
	//
	if((NULL == node_ptr) || (NULL == pre_image) || (pre_image_len == 0) || (NULL == sig) || (NULL == sig_len))
	{
		return -1;
	}

	uint8_t sig_raw[65] = {'\0'};
	uint8_t sig_der[72] = {'\0'};//hex:48:4*16+8
	uint8_t pby;
	//
	int ret = -1;
	ret = hdnode_sign(node_ptr, (const uint8_t *)pre_image, pre_image_len, HASHER_SHA2D, sig_raw, &pby, NULL);
	if((ret == 1) || (ret > 65))
	{
		return -1;
	}
	

#if 0
	int i = 0;
	printf("hdnode_sign():ret:%d\n", ret);
	for(i = 0; i < 65; i ++)
	{
		printf("%02x", sig_raw[i]);
	}
	printf("\n");
#endif

	ret = ecdsa_sig_to_der(sig_raw, sig_der);
	if((ret == 1) || (ret > 72))
	{
		return -1;
	}

#if 1
	printf("der:ret:%d\n", ret);
	for(int i = 0; i < 72; i ++)
	{
		printf("%02x", sig_der[i]);
	}
	printf("\n");
#endif
	
	//copy to buffer
	memcpy(sig, sig_der, ret);
	sig[ret] = 0x01;
	uint32_t sig_len_32 = ret+1;
	memcpy(sig_len, &sig_len_32, sizeof(uint32_t));

	return 0;
}


int get_sigs_list(SPC_WITNESS *witness_st_ptr, SPC_SIGS_LIST *sigs_list_st_ptr)
{
	//input check
	if((NULL == witness_st_ptr) || (NULL == sigs_list_st_ptr))
	{
		printf("spcsign.c:get_sigs_list():get null input.\n");
		return -1;
	}

	memset(sigs_list_st_ptr, '\0', sizeof(SPC_SIGS_LIST));

	//get every item in witness list
	uint64_t op_items_counter = 0;
	op_items_counter = witness_st_ptr->op_items_counter;
	if(op_items_counter != 5)	
	{
		printf("get_sigs_list():wrong items num in witness.\n");
		return -1;

	}

	//check sigs num;	
	int check_flag = 0;
	OP_ITEM op_item_st;
	int i = 0;
	for(i = 0; i < 3; i++)
	{
		memset(&op_item_st, '\0', sizeof(OP_ITEM));
		memcpy(&op_item_st, (witness_st_ptr->op_items_list) + (i+1)*sizeof(OP_ITEM) , sizeof(OP_ITEM));
		if(op_item_st.op_item_len > 1)
		{
			check_flag ++;
		}
	}

	if(check_flag != 1)
	{
		printf("get_sigs_list():wrong num of sig.\n");
		return -1;
	}


	//for eche, copy sigs from witness to sigs list;
	SPC_SIGS_LIST sigs_list;
	memset(&sigs_list, '\0', sizeof(SPC_SIGS_LIST));
	sigs_list.sigs_counter = 0;
	sigs_list.sigs = NULL;
	memcpy(sigs_list_st_ptr, &sigs_list, sizeof(SPC_SIGS_LIST));
	SPC_SIG sig_st;
	uint8_t *sig_ptr = NULL;
	size_t sig_len_size_t = 0;
	uint8_t *sigs = NULL;
	sigs = malloc(3*sizeof(SPC_SIG));
	if(NULL == sigs)
	{
		printf("get_sigs_list():Failed to malloc for sigs.\n");
		return -1;	
	}
	memset(sigs, '\0', 3*sizeof(SPC_SIG));
	sigs_list.sigs = sigs;

	for(i = 0; i < 3; i++)
	{
		memset(&op_item_st, '\0', sizeof(OP_ITEM));
		memcpy(&op_item_st, (witness_st_ptr->op_items_list) + (i+1)*sizeof(OP_ITEM) , sizeof(OP_ITEM));
		sig_len_size_t = 0;		
		sig_len_size_t = op_item_st.op_item_len;
		sig_ptr = malloc(sig_len_size_t);
		if(NULL == sig_ptr)
		{
			printf("get_sigs_list():Failed to malloc for one sig.\n");
			break;				
		}
		memset(sig_ptr, '\0', sig_len_size_t);
		memset(&sig_st, '\0', sizeof(SPC_SIG));
		sig_st.sig_len = sig_len_size_t;		
		memcpy(sig_ptr, op_item_st.op_item, sig_len_size_t);
		sig_st.sig = sig_ptr;

		memcpy(sigs+i*sizeof(SPC_SIG), &sig_st, sizeof(SPC_SIG));
		sigs_list.sigs_counter ++;

	}

	memcpy(sigs_list_st_ptr, &sigs_list, sizeof(SPC_SIGS_LIST));
	
	if((sigs_list.sigs_counter) != 3)
	{
		return -1;
	}
 
	return 0;
}


void free_sigs_list(SPC_SIGS_LIST *sigs_list_st)
{
	//input check
	if((NULL == sigs_list_st) || ((sigs_list_st->sigs_counter) > 3))
	{
		printf("free_sigs_list():input check failed.\n");
		return;
	}

	//
	SPC_SIG sig_st;
	memset(&sig_st, '\0', sizeof(SPC_SIG));
	uint64_t sigs_counter = sigs_list_st->sigs_counter;

	uint64_t i = 0;
	for(i = 0; i < sigs_counter; i ++)
	{
		memcpy(&sig_st, (sigs_list_st->sigs) + i*sizeof(SPC_SIG), sizeof(SPC_SIG));
		free(sig_st.sig);
	}
	
	free(sigs_list_st->sigs);	

	return;
}


int insert_new_sig_to_list(uint8_t *new_sig, uint64_t new_sig_len, SPC_SIGS_LIST *sigs_list, uint8_t order)
{
	//input check
	if((NULL == new_sig) || (new_sig_len > 72) || (new_sig_len < 71) || (NULL == sigs_list) || (order > 2))
	{
		printf("insert_new_sig_to_list():input check failed.\n");
		return -1;
	}

	//check pos
	SPC_SIG sig_st;
	memcpy(&sig_st, (sigs_list->sigs) + order*sizeof(SPC_SIG), sizeof(SPC_SIG));
	if(sig_st.sig_len != 1)
	{
		printf("insert_new_sig_to_list():wrong order.\n");
		return -1;
	}

	//malloc and inset;
	uint8_t *new_sig_ptr = NULL;
	size_t new_sig_len_size_t = new_sig_len;
	new_sig_ptr = malloc(new_sig_len_size_t);
	if(NULL == new_sig_ptr)
	{
		printf("insert_new_sig_to_list():failed to malloc for new sig.\n");
		return -1;
	}

	memcpy(new_sig_ptr, new_sig, new_sig_len_size_t);
	sig_st.sig_len = new_sig_len_size_t;
	sig_st.sig = new_sig_ptr;
	memcpy((sigs_list->sigs) + order*sizeof(SPC_SIG), &sig_st, sizeof(SPC_SIG));

	return 0;
}

//
void debug_print_sigs_list(SPC_SIGS_LIST *sig_list)
{
	SPC_SIG sig_st;
	int i = 0;
	uint64_t j = 0;
	for(i = 0; i < 3; i ++)
	{
		memcpy(&sig_st, (sig_list->sigs + i*sizeof(SPC_SIG)), sizeof(SPC_SIG));
		for(j = 0; j < sig_st.sig_len; j++)
		{
			printf("%02x", (sig_st.sig)[j]);
		}
		printf("\n");
	}

	return;
}



//
int gen_new_witness(uint8_t *new_sig, uint64_t new_sig_len, uint8_t order, SPC_WITNESS *old_witness_st, SPC_WITNESS *new_witness_st)
{
	//inputs check
	if((NULL == new_sig) || (new_sig_len > 113) || (order > 2) || (NULL == old_witness_st) || (NULL == new_witness_st))
	{
		printf("gen_new_witness():input check failed.\n");
		return -1;
	}

	memset(new_witness_st, '\0', sizeof(SPC_WITNESS));

	//get sig list
	int ret = -1;
	SPC_SIGS_LIST sigs_list;
	memset(&sigs_list, '\0', sizeof(SPC_SIGS_LIST));
	ret = get_sigs_list(old_witness_st, &sigs_list);
	if(ret == -1)
	{
		free_sigs_list(&sigs_list);
		return -1;
	}

	printf("DEBUG:GEN_NEW_WITNESS():OLD_SIG_LIST:\n");
	debug_print_sigs_list(&sigs_list);

	//insert new sig
	ret = insert_new_sig_to_list(new_sig, new_sig_len, &sigs_list, order);
	if(ret == -1)
	{
		free_sigs_list(&sigs_list);
		return -1;
	}

	printf("DEBUG:GEN_NEW_WITNESS():NEW_SIG_LIST:\n");
	debug_print_sigs_list(&sigs_list);

	//get script code
	uint8_t scriptcode_buf[106] = {'\0'};//0x69+105
	ret = gen_scriptcode(old_witness_st, scriptcode_buf, 106);
	if(ret == -1)
	{
		free_sigs_list(&sigs_list);
		return -1;
	}


	//gen new witness st
	OP_ITEM op_item_st;
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	SPC_WITNESS witness_st;
	memset(&witness_st, '\0', sizeof(SPC_WITNESS));
	uint8_t *op_items_list_ptr = NULL;
	op_items_list_ptr = malloc(4*sizeof(OP_ITEM));
	if(NULL == op_items_list_ptr)
	{
		free_sigs_list(&sigs_list);
		return -1;
	}	
	memset(op_items_list_ptr, '\0', 4*sizeof(OP_ITEM));

	//malloc
	//op_st
	//copy to list buffer
	////none
	uint8_t *item_ptr = NULL;
	uint8_t *sig1_ptr = NULL;
	uint8_t *sig2_ptr = NULL;
	size_t item_len = 0;
	op_item_st.op_item_len = 0;
	op_item_st.op_item = NULL;
	memcpy(op_items_list_ptr, &op_item_st, sizeof(OP_ITEM));

	////sig1
	int next_sig = 0;
	SPC_SIG sig1_st;
	memset(&sig1_st, '\0', sizeof(SPC_SIG));
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	memcpy(&sig1_st, sigs_list.sigs, sizeof(SPC_SIG));
	item_ptr = NULL;
	if((sig1_st.sig_len != 1) && (NULL != sig1_st.sig))
	{
		next_sig = 1;
		op_item_st.op_item_len = sig1_st.sig_len;
		item_len = op_item_st.op_item_len;
		item_ptr = malloc(item_len);
		if(NULL == item_ptr)
		{
			free_sigs_list(&sigs_list);
			//free() none;
			free(op_items_list_ptr);
			return -1;
		}
		memset(item_ptr, '\0', item_len);
		memcpy(item_ptr, sig1_st.sig, item_len);
		op_item_st.op_item = item_ptr;
		sig1_ptr = item_ptr;
	}
	else
	{
		memset(&sig1_st, '\0', sizeof(SPC_SIG));
		memset(&op_item_st, '\0', sizeof(OP_ITEM));
		memcpy(&sig1_st, (sigs_list.sigs) + sizeof(SPC_SIG), sizeof(SPC_SIG));
		item_ptr = NULL;
		if((sig1_st.sig_len != 1) && (NULL != sig1_st.sig))
		{
			next_sig = 2;
			op_item_st.op_item_len = sig1_st.sig_len;
			item_len = op_item_st.op_item_len;
			item_ptr = malloc(item_len);
			if(NULL == item_ptr)
			{
				free_sigs_list(&sigs_list);
				//free() none;
				free(op_items_list_ptr);
				return -1;
			}
			memset(item_ptr, '\0', item_len);
			memcpy(item_ptr, sig1_st.sig, item_len);
			op_item_st.op_item = item_ptr;
			sig1_ptr = item_ptr;
		}
		else
		{
			free_sigs_list(&sigs_list);
			//free() none;
			free(op_items_list_ptr);
			return -1;
		}		

	}
	//copy to op_items_list buffer
	memcpy(op_items_list_ptr + sizeof(OP_ITEM), &op_item_st, sizeof(OP_ITEM));
	
	
	////sig2
	SPC_SIG sig2_st;
	memset(&sig2_st, '\0', sizeof(SPC_SIG));
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	if(next_sig == 1)
	{
		memcpy(&sig2_st, (sigs_list.sigs) + sizeof(SPC_SIG), sizeof(SPC_SIG));
		item_ptr = NULL;	
		if((sig2_st.sig_len != 1) && (NULL != sig2_st.sig))
		{
			op_item_st.op_item_len = sig2_st.sig_len;
			item_len = op_item_st.op_item_len;
			item_ptr = malloc(item_len);
			if(NULL == item_ptr)
			{
				free_sigs_list(&sigs_list);
				//free() none;
				free(sig1_ptr);
				free(op_items_list_ptr);
				return -1;
			}
			memset(item_ptr, '\0', item_len);
			memcpy(item_ptr, sig2_st.sig, item_len);
			op_item_st.op_item = item_ptr;
			sig2_ptr = item_ptr;
		}
		else
		{
			memset(&sig2_st, '\0', sizeof(SPC_SIG));
			memset(&op_item_st, '\0', sizeof(OP_ITEM));
			memcpy(&sig2_st, (sigs_list.sigs) + 2*sizeof(SPC_SIG), sizeof(SPC_SIG));
			item_ptr = NULL;
			if((sig2_st.sig_len != 1) && (NULL != sig2_st.sig))
			{
				op_item_st.op_item_len = sig2_st.sig_len;
				item_len = op_item_st.op_item_len;
				item_ptr = malloc(item_len);
				if(NULL == item_ptr)
				{
					free_sigs_list(&sigs_list);
					//free() none;
					free(sig1_ptr);
					free(op_items_list_ptr);
					return -1;
				}
				memset(item_ptr, '\0', item_len);
				memcpy(item_ptr, sig2_st.sig, item_len);
				op_item_st.op_item = item_ptr;
				sig2_ptr = item_ptr;
			}
			else
			{
				free_sigs_list(&sigs_list);
				//free() none;
				free(sig1_ptr);
				free(op_items_list_ptr);
				return -1;
			}		
			
			
		}
		
	}
	else if(next_sig == 2)
	{
		memset(&sig2_st, '\0', sizeof(SPC_SIG));
		memset(&op_item_st, '\0', sizeof(OP_ITEM));
		memcpy(&sig2_st, (sigs_list.sigs) + 2*sizeof(SPC_SIG), sizeof(SPC_SIG));
		item_ptr = NULL;
		if((sig2_st.sig_len != 1) && (NULL != sig2_st.sig))
		{
			op_item_st.op_item_len = sig2_st.sig_len;
			item_len = op_item_st.op_item_len;
			item_ptr = malloc(item_len);
			if(NULL == item_ptr)
			{
				free_sigs_list(&sigs_list);
				//free() none;
				free(sig1_ptr);
				free(op_items_list_ptr);
				return -1;
			}
			memset(item_ptr, '\0', item_len);
			memcpy(item_ptr, sig2_st.sig, item_len);
			op_item_st.op_item = item_ptr;
			sig2_ptr = item_ptr;
		}
		else
		{
			free_sigs_list(&sigs_list);
			//free() none;
			free(sig1_ptr);
			free(op_items_list_ptr);
			return -1;
		}		
			
	}
	else
	{
		free_sigs_list(&sigs_list);
		//free() none;
		free(sig1_ptr);
		free(op_items_list_ptr);
		return -1;
	}
	//copy to op_items_list buffer
	memcpy(op_items_list_ptr + 2*sizeof(OP_ITEM), &op_item_st, sizeof(OP_ITEM));
	
	//free sig list
	free_sigs_list(&sigs_list);

	////scriptcode	
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	op_item_st.op_item_len = 105;
	uint8_t *script_code_ptr = NULL;
	script_code_ptr = malloc(105);
	if(NULL == script_code_ptr)
	{
		free_sigs_list(&sigs_list);
		free(sig1_ptr);
		free(sig2_ptr);
		free(op_items_list_ptr);
		return -1;
	}
	
	memcpy(script_code_ptr, scriptcode_buf+1, 105);
	op_item_st.op_item = script_code_ptr;
	memcpy(op_items_list_ptr + 3*sizeof(OP_ITEM), &op_item_st, sizeof(OP_ITEM));

	//copy witness st to given ptr
	witness_st.op_items_counter = 4;
	witness_st.op_items_list = op_items_list_ptr;
	memcpy(new_witness_st, &witness_st, sizeof(SPC_WITNESS));

	return 0;
}


//partial witness and signed witness are both OK, what a fucking nice func in the world;
int get_sigs_num_from_witness(SPC_WITNESS *witness_st)
{
	//check inputs
	if(NULL == witness_st)
	{
		return -1;
	}

	int partial_flag = 0;

	//check items num
	if((witness_st->op_items_counter) == 5)
	{
		partial_flag = 1;
	}
	else if((witness_st->op_items_counter) == 4)
	{
		partial_flag = 0;
	}
	else
	{
		return -1;
	}

	//printf("get_sigs_num_from_witness():partial flag:%d\n", partial_flag);

	//get check round
	int check_round = 0;
	if(partial_flag == 1)
	{
		check_round = 3;
	}
	else if(partial_flag == 0)
	{
		check_round = 2;
	}
	else
	{
		return -1;
	} 

	//check op_item1 and op_item2's len to get sigs num;
	OP_ITEM op_item_st;
	
	int i = 0;
	int sigs_num = 0;
	for(i = 0; i < check_round; i ++)
	{
		//get one item
		memset(&op_item_st, '\0', sizeof(OP_ITEM));
		memcpy(&op_item_st, (witness_st->op_items_list) + (i+1)*sizeof(OP_ITEM), sizeof(OP_ITEM));
	
		//check length
		//printf("get op_item_len:%ld\n", op_item_st.op_item_len);
		if(((op_item_st.op_item_len) > 70) && ((op_item_st.op_item_len) < 105))
		{
			//printf("condition in once.\n");
			sigs_num++;
			//printf("once sigs_num:%d.\n", sigs_num);
		}
	}

	//printf("get sigs_num:%d\n", sigs_num);

	return sigs_num;

	//return 0;
}


//static
int update_tx_witness(SPC_TX *tx_st, SPC_WITNESS *new_witness_st, uint64_t witness_index)
{
	//check inputs
	if((NULL == tx_st) || (NULL == new_witness_st))
	{
		return -1;
	}
	
	if(witness_index > ((tx_st->inputs_counter) - 1))
	{
		return -1;
	}

	//copy to witness list in pos
	memcpy((tx_st->witnesses_list) + witness_index*sizeof(SPC_WITNESS), new_witness_st, sizeof(SPC_WITNESS));
	
	//return;
	return 0;
}


//static 
int free_partial_witness(SPC_WITNESS *witness_st)
{
	//check input
	if(NULL == witness_st)
	{
		return -1;
	}

	if((witness_st->op_items_counter) != 5)
	{
		return -1;
	}

	free_single_witness(witness_st);

	return 0;
}


////
int update_tx_after_signed(SPC_TX *tx_st, SPC_WITNESS *new_witness_st, uint64_t witness_index)
{
	//chcek inputs
	if((NULL == tx_st) || (NULL == new_witness_st))
	{
		return -1;
	}

	//printf("update_tx_after_signed():inputs checked.\n");

	//check sigs num both old witness and new_witness
	int sigs_num = 0;
	sigs_num = get_sigs_num_from_witness((SPC_WITNESS *)(tx_st->witnesses_list + witness_index*sizeof(SPC_WITNESS)));
	if(sigs_num != 1)
	{
		printf("update_tx_after_signed():get sigs num from old witness:%d.\n", sigs_num);
		return -1;
	}
	
	sigs_num = 0;
	sigs_num = get_sigs_num_from_witness(new_witness_st);
	if(sigs_num != 2)
	{
		printf("update_tx_after_signed():get sigs num from new witness:%d.\n", sigs_num);
		return -1;
	}

	//printf("check 1\n");

	//get old witness 
	SPC_WITNESS old_witness;
	memset(&old_witness, '\0', sizeof(SPC_WITNESS));
	memcpy(&old_witness, (SPC_WITNESS *)(tx_st->witnesses_list + witness_index*sizeof(SPC_WITNESS)), sizeof(SPC_WITNESS));

	//update tx
	int ret = -1;
	ret = update_tx_witness(tx_st, new_witness_st, witness_index);
	if(ret == -1)
	{
		return -1;
	}

	//printf("check 2\n");

	//free old witness
	ret = free_partial_witness(&old_witness);
	if(ret == -1)
	{
		return -1;
	}

	//printf("check 3\n");

	return 0;
}

//
int sign_single_input(uint8_t *xpub, uint32_t xpub_len, uint8_t *xprv, uint32_t xprv_len, SPC_TX *tx_st, uint64_t witness_index)
{
	//check inputs
	if((NULL == xpub) || (xpub_len > 113) || (NULL == xprv) || (xprv_len > 113) || (NULL == tx_st))
	{
		return -1;
	}

	if(witness_index > (tx_st->inputs_counter - 1))
	{
		return -1;
	}
	

	int ret = -1;
	//get_order_and_paths
	uint8_t order = 3;
	uint32_t path1 = 0;
	uint32_t path2 = 0;
	ret = get_order_and_paths(xpub, xpub_len, (SPC_WITNESS *)((tx_st->witnesses_list) + witness_index*sizeof(SPC_WITNESS)), &order, &path1, &path2);
	if(-1 == ret)
	{
		printf("sign_signle_input():get_order_and_paths() failed.\n");
		return -1;
	}

	printf("DEBUG:SIGN_SINGLE_INPUT():ORDER:%d.\n", order);

	//gen_pre_image
	uint8_t pre_image_buffer[262] = {'\0'};
	ret = gen_preimage((uint8_t *)(&(tx_st->nversion)), tx_st->inputs_list, tx_st->inputs_counter, tx_st->outputs_list, tx_st->outputs_counter, (SPC_WITNESS *)(tx_st->witnesses_list + witness_index*sizeof(SPC_WITNESS)), witness_index, (uint8_t *)(&(tx_st->lock_time)), pre_image_buffer, 262);
	if(-1 == ret)
	{
		printf("sign_single_input():gen_preiamge() failed.\n");
		return -1;
	}	
	
	printf("DEBUG:SIGN_SINGLE_INPUT():GEN PREIMAGE DONE\n");
	
	//get prv node
	HDNode sign_node_st;
	memset(&sign_node_st, '\0', sizeof(HDNode));
	ret = get_node_from_xprv_double_ckd(xprv, xprv_len, path1, path2, &sign_node_st);
	if(-1 == ret)
	{
		printf("sign_single_input():get_node_from_xprv_double_ckd() failed.\n");
		return -1;
	}

	//gen_sig
	uint8_t sig_buffer[73] = {'\0'};
	uint32_t sig_len = 0;
	ret = gen_sig(&sign_node_st, pre_image_buffer, 262, sig_buffer, &sig_len);
	if(-1 == ret)
	{
		printf("sign_single_input():gen_sig() failed.\n");
		return -1;
	}
	if(sig_len < 70)
	{
		printf("sign_single_input():wrong sig_len.\n");
		return -1;
	}


	//gen_new_witness
	uint64_t sig_len_64 = 0;
	sig_len_64 = sig_len;
	SPC_WITNESS new_witness_st;
	memset(&new_witness_st, '\0', sizeof(SPC_WITNESS));
	ret = gen_new_witness(sig_buffer, sig_len_64, order, (SPC_WITNESS *)(tx_st->witnesses_list + witness_index*sizeof(SPC_WITNESS)), &new_witness_st);
	if(-1 == ret)
	{
		printf("sign_single_input():gen_new_witness() failed.\n");
		return -1;
	}
	
	//update_tx_after_signed
	ret = update_tx_after_signed(tx_st, &new_witness_st, witness_index);
	if(-1 == ret)
	{
		printf("sign_single_input():update_tx_after_signed() failed.\n");
		return -1;
	}

#if 0
	SPC_WITNESS test_wit;
	int i = 0;
	for(i = 0; i < 2; i++)
	{
		memcpy(&test_wit, tx_st->witnesses_list + i*sizeof(SPC_WITNESS), sizeof(SPC_WITNESS));
		printf("test_wit[%d]:op_num:%ld\n", i, test_wit.op_items_counter);
	}
#endif

	return 0;
}

//api
int sign_tx(uint8_t *xpub, uint32_t xpub_len, uint8_t *xprv, uint32_t xprv_len, SPC_TX *tx_st_ptr)
{
	//check inputs
	if((NULL == xpub) || (xpub_len > 113) || (NULL == xprv) || (xprv_len > 113) || (NULL == tx_st_ptr))
	{
		printf("sign_tx():inputs check failed.\n");
		return -1;
	}
	printf("SIGN_TX():CHECK INPUTS DONE\n");
		
	//get inputs num
	uint64_t inputs_num = tx_st_ptr->inputs_counter;
	printf("sign_tx:inputs_num:%ld\n", inputs_num);
	//sign every inputs
	uint64_t i = 0;
	int ret = -1;
	for(i = 0; i < inputs_num; i++)
	{
		printf("SIGN INPUTS:%ld\n", i);
		ret = sign_single_input(xpub, xpub_len, xprv, xprv_len, tx_st_ptr, i);
		if(-1 == ret)
		{
			printf("sign_tx():failed to sign input:%ld.\n", i);
			return -1;
		}
	}

	return 0;
}






