#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "spctools.h"
#include "spcdeserialize.h"

int get_inputs_counter(uint8_t *buffer, uint64_t *inputs_counter_ptr, uint8_t *inputs_counter_len)
{
	if((buffer == NULL) || (inputs_counter_ptr == NULL) || (inputs_counter_len == NULL))	
	{
		return -1;	
	}

	uint8_t head = 0;
	head = read_varint_len(buffer);
	if(head == 0)
	{
		return -1;
	}


	uint64_t inputs_counter = 0;
	int ret = -1;
	if(head < 0xfd)
	{
		inputs_counter = head;
		memcpy(inputs_counter_ptr, &inputs_counter, sizeof(uint64_t));
		*inputs_counter_len = 1;
		return 0;
	}
	else if(head == 0xfd)
	{
		*inputs_counter_len = 3;
		ret = read_varint(buffer+1, 2, (uint8_t *)(&inputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(inputs_counter_ptr, &inputs_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xfe)
	{
		*inputs_counter_len = 5;
		ret = read_varint(buffer+1, 4, (uint8_t *)(&inputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(inputs_counter_ptr, &inputs_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xff)
	{
		*inputs_counter_len = 9;
		ret = read_varint(buffer+1, 8, (uint8_t *)(&inputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(inputs_counter_ptr, &inputs_counter, sizeof(uint64_t));
		return 0;

	}
	else
	{
//		printf("deserialize_tx.c:get_inputs_counter():should not be hear.");
		return -1;
	}

}


//get single input
int get_single_input(uint8_t *buffer, SPC_INPUT *input_st_ptr, uint64_t *counter_ptr)
{
	if((buffer == NULL) || (input_st_ptr == NULL) || (counter_ptr == NULL))
	{
		return -1;
	}
//	printf("get_single_input():input check done.\n");


	uint64_t counter = 0;
	int ret = -1;

	//get prevhash point
	uint8_t get_tx_hash[PREVTXHASHLEN] = {'\0'};
	//uint8_t reversed_tx_hash[PREVTXHASHLEN] = {'\0'};
	memcpy(get_tx_hash, buffer, PREVTXHASHLEN);
	#if 0
	//no need to reverse;
	ret = array_reverse(PREVTXHASHLEN, get_tx_hash, PREVTXHASHLEN, reversed_tx_hash, 1);
	if(ret != 0)
	{
		return -1;
	}	
	memcpy(input_st_ptr->prev_tx_hash, reversed_tx_hash, PREVTXHASHLEN);
	#endif
	memcpy(input_st_ptr->prev_tx_hash, get_tx_hash, PREVTXHASHLEN);
	counter += PREVTXHASHLEN;
	memcpy(input_st_ptr->prev_tx_index, buffer+counter, PREVTXINDEXLEN);
	counter += PREVTXINDEXLEN;
//	printf("get single input():get hash point done.\n");	
	
	//get txin script
	//get txin script len
	uint8_t head = 0;
	//uint8_t len_size = 0;
	uint64_t txin_script_len = 0;
	head = read_varint_len(buffer+counter);
	counter += sizeof(uint8_t);
	if(head < 0xfd)
	{	
		txin_script_len = head;
		memcpy(&(input_st_ptr->txin_script_len), &txin_script_len, sizeof(uint64_t));  

	}
	else if(head == 0xfd)
	{
		uint16_t u16_len = 0;
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&u16_len));
		if(ret == -1)
		{
			memset(input_st_ptr, '\0', sizeof(SPC_INPUT));
			return -1;
		}

		counter += 2;
		txin_script_len = u16_len;
		memcpy(&(input_st_ptr->txin_script_len), &txin_script_len, sizeof(uint64_t));  
	}
	else if(head == 0xfe)
	{
		uint32_t u32_len = 0;
		ret = read_varint(buffer+counter, 4, (uint8_t *)(&u32_len));
		if(ret == -1)
		{
			memset(input_st_ptr, '\0', sizeof(SPC_INPUT));
			return -1;
		}

		counter += 4;
		txin_script_len = u32_len;
		memcpy(&(input_st_ptr->txin_script_len), &txin_script_len, sizeof(uint64_t));  

	}
	else if(head == 0xff)
	{
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&txin_script_len));
		if(ret == -1)
		{
			memset(input_st_ptr, '\0', sizeof(SPC_INPUT));
			return -1;
		}

		counter += 8;
		memcpy(&(input_st_ptr->txin_script_len), &txin_script_len, sizeof(uint64_t));  

	}

//	printf("get single input():got txin script length\n");

	//get txinscript context
	uint8_t *txin_script_ptr = NULL;
	txin_script_ptr = malloc(txin_script_len);
	if(txin_script_ptr == NULL)
	{
//		printf("got null for txin script.\n");
		memset(input_st_ptr, '\0', sizeof(SPC_INPUT));
		return -1;
	}
//	printf("txin_script_ptr:%p\n", txin_script_ptr);
//	printf("txin_script_len:%lu\n", txin_script_len);
	memcpy(txin_script_ptr, buffer+counter, txin_script_len);
//	printf("copied txin_script to txin script buffer.\n");
	memcpy(&(input_st_ptr->txin_script), &(txin_script_ptr), sizeof(uint8_t *));
	counter += txin_script_len;
	
//	printf("get single input():get txin script done.\n");



	//get sequence_no
	memcpy(input_st_ptr->sequence_no, buffer+counter, 4);
	counter += 4;

	memcpy(counter_ptr, &counter, sizeof(uint64_t));

	return 0;
}


int init_inputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *inputs_counter_ptr, uint8_t **inputs_list_ptr, uint64_t *inputs_len_ptr)
{
	if((buffer == NULL) || (buffer_len < 6) || (inputs_counter_ptr == NULL) || (inputs_list_ptr == NULL) || (inputs_len_ptr == NULL))
	{
		return -1;
	}

//	printf("input check done\n");

	uint64_t counter = 0;
	uint8_t inputs_counter_len = 0;
	int ret = -1;
	uint64_t inputs_num = 0;
	ret = get_inputs_counter(buffer, &inputs_num, &inputs_counter_len);
//	printf("get_inputs_counter():ret:%d\n", ret);
//	printf("inputs_num:%lu\n", inputs_num);
	if(ret != 0)
	{
		*inputs_counter_ptr = 0;
		*inputs_list_ptr = NULL;
		return -1;
	}
	else if(buffer_len < (inputs_num*(32+4+1+4)))
	{
		*inputs_counter_ptr = 0;
		*inputs_list_ptr = NULL;
		return -1;
	}
	else
	{
		*inputs_counter_ptr = inputs_num;
		counter += inputs_counter_len; 
	}	

//	printf("got inputs num.\n");
	
	//malloc buffer
	uint8_t *inputs_buffer = NULL;
	inputs_buffer = malloc(inputs_num*sizeof(SPC_INPUT));
	if(inputs_buffer == NULL)
	{
		
		*inputs_counter_ptr = 0;
		*inputs_list_ptr = NULL;
		return -2;
	}
	memset(inputs_buffer, '\0', inputs_num*sizeof(SPC_INPUT));

//	printf("inputs_buffer init done.\n");

	//get_single_input();
	uint64_t i = 0;
	SPC_INPUT input_st;
	memset(&input_st, '\0', sizeof(SPC_INPUT));
	uint64_t each_input_len = 0;
//	printf("ready to get each input.\n");
	for(i = 0; i < inputs_num; i++)
	{
//		printf("i:%lu\n", i);
		ret = get_single_input(buffer+counter, &input_st, &each_input_len);
		if((ret != 0) || (each_input_len == 0)) 
		{
//			printf("get single input error, %d\n", ret);
			//free(inputs_buffer);
			*inputs_counter_ptr = i;
			*inputs_list_ptr = inputs_buffer;
			return -3;
		}
//		printf("get_single_input():ret:%d\n", ret);

		counter += each_input_len;
//		printf("here?\n");
		memcpy(inputs_buffer + i*sizeof(SPC_INPUT), &input_st, sizeof(SPC_INPUT)); 
//		printf("one input done.\n");
	}
//	printf("all the inputs put into buffer\n");

//	printf("inputs_buffer:%p\n", inputs_buffer);
	*inputs_list_ptr = inputs_buffer;
	*inputs_len_ptr = counter;

	return 0;
}


void free_inputs(uint8_t *inputs_list, uint64_t inputs_counter)
{
	if(inputs_list == NULL)
	{
		return;
	}

//	printf("free_inputs():input check done.\n");	

	SPC_INPUT input_st;
	uint64_t i = 0;
	for(i = 0; i < inputs_counter; i ++)
	{
		memcpy(&input_st, inputs_list+i*sizeof(SPC_INPUT), sizeof(SPC_INPUT));
		
//		printf("txin_script:%p\n", input_st.txin_script);
		if(input_st.txin_script_len == 0)
		{
			continue;
		}else
		{
			free(input_st.txin_script);
		}
	}

//	printf("txin script free done\n");

	free(inputs_list);
//	printf("freed\n");
	return;
}


//output part
//get output counter
//get single output
//init outputs
//free outputs 
int get_outputs_counter(uint8_t *buffer, uint64_t *outputs_counter_ptr, uint8_t *outputs_counter_len)
{
	if((buffer == NULL) || (outputs_counter_ptr == NULL) || (outputs_counter_len == NULL))	
	{
		return -1;	
	}

	uint8_t head = 0;
	head = read_varint_len(buffer);
	if(head == 0)
	{
		return -1;
	}


	uint64_t outputs_counter = 0;
	int ret = -1;
	if(head < 0xfd)
	{
		outputs_counter = head;
		memcpy(outputs_counter_ptr, &outputs_counter, sizeof(uint64_t));
		*outputs_counter_len = 1;
		return 0;
	}
	else if(head == 0xfd)
	{
		*outputs_counter_len = 3;
		ret = read_varint(buffer+1, 2, (uint8_t *)(&outputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(outputs_counter_ptr, &outputs_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xfe)
	{
		*outputs_counter_len = 5;
		ret = read_varint(buffer+1, 4, (uint8_t *)(&outputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(outputs_counter_ptr, &outputs_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xff)
	{
		*outputs_counter_len = 9;
		ret = read_varint(buffer+1, 8, (uint8_t *)(&outputs_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(outputs_counter_ptr, &outputs_counter, sizeof(uint64_t));
		return 0;

	}
	else
	{
//		printf("deserialize_tx.c:get_outputs_counter():should not be here.");
		return -1;
	}

}


//get single output
int get_single_output(uint8_t *buffer, SPC_OUTPUT *output_st_ptr, uint64_t *counter_ptr)
{
	if((buffer == NULL) || (output_st_ptr == NULL) || (counter_ptr == NULL))
	{
		return -1;
	}
//	printf("get_single_output():output check done.\n");


	uint64_t counter = 0;
	int ret = -1;

	//get value
	uint8_t get_value[VALUELEN] = {'\0'};
	memcpy(get_value, buffer, VALUELEN);	

	memcpy(output_st_ptr->value, get_value, VALUELEN);
	counter += VALUELEN;
//	printf("get single output():get value done.\n");	
	
	//get txout script
	//get txout script len
	uint8_t head = 0;
	//uint8_t len_size = 0;
	uint64_t txout_script_len = 0;
	head = read_varint_len(buffer+counter);
	counter += sizeof(uint8_t);
	if(head < 0xfd)
	{	
		txout_script_len = head;
		memcpy(&(output_st_ptr->txout_script_len), &txout_script_len, sizeof(uint64_t));  

	}
	else if(head == 0xfd)
	{
		uint16_t u16_len = 0;
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&u16_len));
		if(ret == -1)
		{
			memset(output_st_ptr, '\0', sizeof(SPC_OUTPUT));
			return -1;
		}

		counter += 2;
		txout_script_len = u16_len;
		memcpy(&(output_st_ptr->txout_script_len), &txout_script_len, sizeof(uint64_t));  
	}
	else if(head == 0xfe)
	{
		uint32_t u32_len = 0;
		ret = read_varint(buffer+counter, 4, (uint8_t *)(&u32_len));
		if(ret == -1)
		{
			memset(output_st_ptr, '\0', sizeof(SPC_OUTPUT));
			return -1;
		}

		counter += 4;
		txout_script_len = u32_len;
		memcpy(&(output_st_ptr->txout_script_len), &txout_script_len, sizeof(uint64_t));  

	}
	else if(head == 0xff)
	{
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&txout_script_len));
		if(ret == -1)
		{
			memset(output_st_ptr, '\0', sizeof(SPC_OUTPUT));
			return -1;
		}

		counter += 8;
		memcpy(&(output_st_ptr->txout_script_len), &txout_script_len, sizeof(uint64_t));  

	}

//	printf("get single output():got txout script length\n");

	//get txoutscript context
	uint8_t *txout_script_ptr = NULL;
	txout_script_ptr = malloc(txout_script_len);
	if(txout_script_ptr == NULL)
	{
//		printf("got null for txout script.\n");
		memset(output_st_ptr, '\0', sizeof(SPC_OUTPUT));
		return -1;
	}
//	printf("txout_script_ptr:%p\n", txout_script_ptr);
//	printf("txout_script_len:%lu\n", txout_script_len);
	memcpy(txout_script_ptr, buffer+counter, txout_script_len);
//	printf("copied txout_script to txout script buffer.\n");
	memcpy(&(output_st_ptr->txout_script), &(txout_script_ptr), sizeof(uint8_t *));
	counter += txout_script_len;
	
//	printf("get single output():get txout script done.\n");


	memcpy(counter_ptr, &counter, sizeof(uint64_t));

	return 0;
}


int init_outputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *outputs_counter_ptr, uint8_t **outputs_list_ptr, uint64_t *outputs_len_ptr)
{
	if((buffer == NULL) || (buffer_len < 6) || (outputs_counter_ptr == NULL) || (outputs_list_ptr == NULL) || (outputs_len_ptr == NULL))
	{
		return -1;
	}

//	printf("input check done\n");

	uint64_t counter = 0;
	uint8_t outputs_counter_len = 0;
	int ret = -1;
	uint64_t outputs_num = 0;
	ret = get_outputs_counter(buffer, &outputs_num, &outputs_counter_len);
//	printf("get_outputs_counter():ret:%d\n", ret);
//	printf("outputs_num:%lu\n", outputs_num);
	if(ret != 0)
	{
		*outputs_counter_ptr = 0;
		*outputs_list_ptr = NULL;
		return -1;
	}
	else if(buffer_len < (outputs_num*(8+1)))
	{
		*outputs_counter_ptr = 0;
		*outputs_list_ptr = NULL;
		return -1;
	}
	else
	{
		*outputs_counter_ptr = outputs_num;
		counter += outputs_counter_len; 
	}	

//	printf("got outputs num.\n");
	
	//malloc buffer
	uint8_t *outputs_buffer = NULL;
	outputs_buffer = malloc(outputs_num*sizeof(SPC_OUTPUT));
	if(outputs_buffer == NULL)
	{
		
		*outputs_counter_ptr = 0;
		*outputs_list_ptr = NULL;
		return -2;
	}
	memset(outputs_buffer, '\0', outputs_num*sizeof(SPC_OUTPUT));

//	printf("outputs_buffer init done.\n");

	//get_single_output();
	uint64_t i = 0;
	SPC_OUTPUT output_st;
	memset(&output_st, '\0', sizeof(SPC_OUTPUT));
	uint64_t each_output_len = 0;
//	printf("ready to get each output.\n");
	for(i = 0; i < outputs_num; i++)
	{
//		printf("i:%lu\n", i);
		ret = get_single_output(buffer+counter, &output_st, &each_output_len);
		if((ret != 0) || (each_output_len == 0)) 
		{
//			printf("get single output error, %d\n", ret);
			//free(outputs_buffer);
			*outputs_counter_ptr = i;
			*outputs_list_ptr = outputs_buffer;
			return -3;
		}
//		printf("get_single_output():ret:%d\n", ret);

		counter += each_output_len;
//		printf("here?\n");
		memcpy(outputs_buffer + i*sizeof(SPC_OUTPUT), &output_st, sizeof(SPC_OUTPUT)); 
//		printf("one output done.\n");
	}
//	printf("all the outputs put into buffer\n");

//	printf("outputs_buffer:%p\n", outputs_buffer);
	*outputs_list_ptr = outputs_buffer;
	*outputs_len_ptr = counter;

	return 0;
}


void free_outputs(uint8_t *outputs_list, uint64_t outputs_counter)
{
	if(outputs_list == NULL)
	{
		return;
	}

//	printf("free_outputs():input check done.\n");	

	SPC_OUTPUT output_st;
	uint64_t i = 0;
	for(i = 0; i < outputs_counter; i ++)
	{
		memcpy(&output_st, outputs_list+i*sizeof(SPC_OUTPUT), sizeof(SPC_OUTPUT));
		
//		printf("txout_script:%p\n", output_st.txout_script);
		if(output_st.txout_script_len == 0)
		{
			continue;
		}
		else
		{
			free(output_st.txout_script);
		}
	}

//	printf("txout script free done\n");

	free(outputs_list);
//	printf("freed\n");
	return;
}


//witness part
//get op items counter
//get single op_item
//init single witness
//free single witness
//init witnesses
//free witnesses


int get_op_items_counter(uint8_t *buffer, uint64_t *op_items_counter_ptr, uint8_t *op_items_counter_len)
{
	if((buffer == NULL) || (op_items_counter_ptr == NULL) || (op_items_counter_len == NULL))	
	{
		return -1;	
	}

	uint8_t head = 0;
	head = read_varint_len(buffer);
	if(head == 0)
	{
		return -1;
	}


	uint64_t op_items_counter = 0;
	int ret = -1;
	if(head < 0xfd)
	{
		op_items_counter = head;
		memcpy(op_items_counter_ptr, &op_items_counter, sizeof(uint64_t));
		*op_items_counter_len = 1;
		return 0;
	}
	else if(head == 0xfd)
	{
		*op_items_counter_len = 3;
		ret = read_varint(buffer+1, 2, (uint8_t *)(&op_items_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(op_items_counter_ptr, &op_items_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xfe)
	{
		*op_items_counter_len = 5;
		ret = read_varint(buffer+1, 4, (uint8_t *)(&op_items_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(op_items_counter_ptr, &op_items_counter, sizeof(uint64_t));
		return 0;

	}
	else if(head == 0xff)
	{
		*op_items_counter_len = 9;
		ret = read_varint(buffer+1, 8, (uint8_t *)(&op_items_counter));
		if(ret != 0)
		{
			return -1;
		}

		memcpy(op_items_counter_ptr, &op_items_counter, sizeof(uint64_t));
		return 0;

	}
	else
	{
//		printf("deserialize_tx.c:get_op_items_counter():should not be here.");
		return -1;
	}

}


//get single op item
int get_single_op_item(uint8_t *buffer, OP_ITEM *op_item_st_ptr, uint64_t *counter_ptr)
//int get_single_output(uint8_t *buffer, SPC_OUTPUT *output_st_ptr, uint64_t *counter_ptr)
{
	if((buffer == NULL) || (op_item_st_ptr == NULL) || (counter_ptr == NULL))
	{
		return -1;
	}
//	printf("get_single_op_item():output check done.\n");

	uint64_t counter = 0;
	int ret = -1;
		
	//get op item len
	uint8_t head = 0;
	//uint8_t len_size = 0;
	uint64_t op_item_len = 0;
	head = read_varint_len(buffer+counter);
	counter += sizeof(uint8_t);
	if(head < 0xfd)
	{	
		op_item_len = head;
		memcpy(&(op_item_st_ptr->op_item_len), &op_item_len, sizeof(uint64_t));  

	}
	else if(head == 0xfd)
	{
		uint16_t u16_len = 0;
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&u16_len));
		if(ret == -1)
		{
			memset(op_item_st_ptr, '\0', sizeof(OP_ITEM));
			return -1;
		}

		counter += 2;
		op_item_len = u16_len;
		memcpy(&(op_item_st_ptr->op_item_len), &op_item_len, sizeof(uint64_t));  
	}
	else if(head == 0xfe)
	{
		uint32_t u32_len = 0;
		ret = read_varint(buffer+counter, 4, (uint8_t *)(&u32_len));
		if(ret == -1)
		{
			memset(op_item_st_ptr, '\0', sizeof(OP_ITEM));
			return -1;
		}

		counter += 4;
		op_item_len = u32_len;
		memcpy(&(op_item_st_ptr->op_item_len), &op_item_len, sizeof(uint64_t));  

	}
	else if(head == 0xff)
	{
		ret = read_varint(buffer+counter, 2, (uint8_t *)(&op_item_len));
		if(ret == -1)
		{
			memset(op_item_st_ptr, '\0', sizeof(OP_ITEM));
			return -1;
		}

		counter += 8;
		memcpy(&(op_item_st_ptr->op_item_len), &op_item_len, sizeof(uint64_t));  

	}

//	printf("get single op item():got op item length\n");

	//get op item context
	uint8_t *op_item_ptr = NULL;
	op_item_ptr = malloc(op_item_len);
	if(op_item_ptr == NULL)
	{
//		printf("got null for op item.\n");
		memset(op_item_st_ptr, '\0', sizeof(OP_ITEM));
		return -1;
	}
//	printf("op_item_ptr:%p\n", op_item_ptr);
//	printf("op_item_len:%lu\n", op_item_len);
	memcpy(op_item_ptr, buffer+counter, op_item_len);
//	printf("copied op_item to op item buffer.\n");
	memcpy(&(op_item_st_ptr->op_item), &(op_item_ptr), sizeof(uint8_t *));
	counter += op_item_len;
	
//	printf("get single op item():get op item context done.\n");

	memcpy(counter_ptr, &counter, sizeof(uint64_t));

	return 0;
}


//init single witness
int init_single_witness(uint8_t *buffer, uint64_t buffer_len, SPC_WITNESS *witness_st_ptr, uint64_t *witnesses_len_ptr)
//int init_outputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *outputs_counter_ptr, uint8_t **outputs_list_ptr, uint64_t *outputs_len_ptr)
{
	if((buffer == NULL) || (buffer_len == 0) || (witness_st_ptr == NULL) || (witnesses_len_ptr == NULL))
	{
		return -1;
	}

//	printf("input check done\n");
	
	uint64_t counter = 0;
	uint8_t op_items_counter_len = 0;
	int ret = -1;
	uint64_t op_items_num = 0;
	uint8_t label_len = 0;
	uint32_t label = 0;
	uint64_t value = 0;
	uint16_t version = 0;
	//check label
	memcpy(&label_len, buffer+counter, sizeof(uint8_t));
	if(label_len != 0xfe)
	{
		return -1;
	}
	counter += sizeof(uint8_t);

	memcpy(&label, buffer+counter, sizeof(uint32_t));
	if(label != 0xffffffff)
	{
		return -1;
	}
	counter += sizeof(uint32_t);

	//get value
	memcpy(&value, buffer+counter, sizeof(uint64_t));
	counter += sizeof(uint64_t);
	memcpy(&(witness_st_ptr->value), &value, sizeof(uint64_t));	
	//get version
	memcpy(&version, buffer+counter, sizeof(uint16_t));
	counter += sizeof(uint16_t);
	if(version != 0)
	{
		return -1;
	}
	memcpy(&(witness_st_ptr->version), &version, sizeof(uint16_t));

	//get op_items
	ret = get_op_items_counter(buffer+counter, &op_items_num, &op_items_counter_len);
//	printf("get_op_items_counter():ret:%d\n", ret);
//	printf("op_items_num:%lu\n", op_items_num);
	if(ret != 0)
	{
		witness_st_ptr->op_items_counter = 0;
		witness_st_ptr->op_items_list = NULL;
		return -1;
	}
	else
	{
		witness_st_ptr->op_items_counter = op_items_num;
		counter += op_items_counter_len; 
	}	

//	printf("got op items num.\n");
	
	//malloc buffer
	uint8_t *op_items_buffer = NULL;
	op_items_buffer = malloc(op_items_num*sizeof(OP_ITEM));
	if(op_items_buffer == NULL)
	{
		witness_st_ptr->op_items_counter = 0;
		witness_st_ptr->op_items_list = NULL;
		return -2;
	}
	memset(op_items_buffer, '\0', op_items_num*sizeof(OP_ITEM));

//	printf("op_items_buffer init done.\n");

	//get_single_op_item();
	uint64_t i = 0;
	OP_ITEM op_item_st;
	memset(&op_item_st, '\0', sizeof(OP_ITEM));
	uint64_t each_op_item_len = 0;
//	printf("ready to get each op item.\n");
	for(i = 0; i < op_items_num; i++)
	{
//		printf("i:%lu\n", i);
		ret = get_single_op_item(buffer+counter, &op_item_st, &each_op_item_len);
		if((ret != 0) || (each_op_item_len == 0)) 
		{
//			printf("get single op item error, %d\n", ret);
			//free(outputs_buffer);
			//*outputs_counter_ptr = i;
			//*outputs_list_ptr = outputs_buffer;
			witness_st_ptr->op_items_counter = i;
			witness_st_ptr->op_items_list = op_items_buffer;
			return -3;
		}
//		printf("get_single_op_item():ret:%d\n", ret);

		counter += each_op_item_len;
//		printf("here?\n");
		memcpy(op_items_buffer + i*sizeof(OP_ITEM), &op_item_st, sizeof(OP_ITEM)); 
//		printf("one op item done.\n");
	}
//	printf("all the op items put into buffer\n");

//	printf("oo_items_buffer:%p\n", op_items_buffer);
	//*outputs_list_ptr = outputs_buffer;
	//*outputs_len_ptr = counter;
	witness_st_ptr->op_items_list = op_items_buffer;
	*witnesses_len_ptr = counter;
	return 0;
}


void free_single_witness(SPC_WITNESS *witness_st_ptr)
{
	if(witness_st_ptr == NULL)
	{
		return;
	}

	OP_ITEM op_item_st;
	uint64_t op_items_num = witness_st_ptr->op_items_counter;
	uint64_t i = 0;
	for(i = 0; i < op_items_num; i ++)
	{
		memcpy(&op_item_st, (witness_st_ptr->op_items_list) + i*sizeof(OP_ITEM), sizeof(OP_ITEM));
		if(op_item_st.op_item_len == 0)
		{
			continue;
		}
		else
		{
			free(op_item_st.op_item);		
		}
	}
	
	free(witness_st_ptr->op_items_list);

	return;
}

int init_witnesses(uint8_t *buffer, uint64_t buffer_len, uint64_t *witnesses_counter_ptr, uint8_t **witnesses_list_ptr, uint64_t *witnesses_len_ptr)
//int init_outputs(uint8_t *buffer, uint64_t buffer_len, uint64_t *outputs_counter_ptr, uint8_t **outputs_list_ptr, uint64_t *outputs_len_ptr)
{
	if((buffer == NULL) || (witnesses_counter_ptr == NULL) || (witnesses_list_ptr == NULL) || (witnesses_len_ptr == NULL))
	{
		return -1;
	}

//	printf("input check done\n");
	uint64_t counter = 0;
	uint64_t witnesses_counter = *witnesses_counter_ptr;
	int ret = -1;

//	printf("witnesses_num:%lu\n", witnesses_counter);
	if(buffer_len < witnesses_counter)
	{
		*witnesses_list_ptr = NULL;
		return -1;
	}
	 
//	printf("buffer len check done.\n");
	
	//malloc buffer
	uint8_t *witnesses_buffer = NULL;
	witnesses_buffer = malloc(witnesses_counter*sizeof(SPC_WITNESS));
	if(witnesses_buffer == NULL)
	{
		*witnesses_list_ptr = NULL;
		return -2;
	}
	memset(witnesses_buffer, '\0', witnesses_counter*sizeof(SPC_WITNESS));

//	printf("witnesses_buffer init done.\n");

	//init_single_witness();
	uint64_t i = 0;
	SPC_WITNESS witness_st;
	memset(&witness_st, '\0', sizeof(SPC_WITNESS));
	uint64_t each_witness_len = 0;
//	printf("ready to get each witness.\n");
	for(i = 0; i < witnesses_counter; i++)
	{
//		printf("i:%lu\n", i);
		ret = init_single_witness(buffer+counter, buffer_len - counter, &witness_st, &each_witness_len);
		if(ret != 0) 
		{
//			printf("init single witness error, %d\n", ret);
			//free(outputs_buffer);
			//free_single_witness(&witness_st);
			*witnesses_counter_ptr = i;
			*witnesses_list_ptr = witnesses_buffer;
			return -3;
		}
//		printf("init_single_witness():ret:%d\n", ret);

		counter += each_witness_len;
//		printf("here?\n");
		memcpy(witnesses_buffer + i*sizeof(SPC_WITNESS), &witness_st, sizeof(SPC_WITNESS)); 
//		printf("one witness done.\n");
		if(counter > buffer_len)
		{
//			printf("init single witness error, %d\n", ret);
			//free(outputs_buffer);
			//free_single_witness(&witness_st);
			*witnesses_counter_ptr = i;
			*witnesses_list_ptr = witnesses_buffer;
			return -3;
		}
		
	}
//	printf("all the witness put into buffer\n");

//	printf("witnesses_buffer:%p\n", witnesses_buffer);
	*witnesses_list_ptr = witnesses_buffer;
	*witnesses_len_ptr = counter;

	return 0;
}



void free_witnesses(uint8_t *witnesses_list, uint64_t witnesses_counter)
{
	if(witnesses_list == NULL)
	{
		return;
	}	

	SPC_WITNESS witness_st;

	uint64_t i = 0;
	for(i = 0; i < witnesses_counter; i ++)
	{
		memcpy(&witness_st, witnesses_list + (i*sizeof(SPC_WITNESS)), sizeof(SPC_WITNESS));
		free_single_witness(&witness_st);
	}

	free(witnesses_list); 

	return;
}


int init_tx(uint8_t *buffer, uint64_t buffer_len, SPC_TX *tx_st_ptr)
{
	//input check
	if((NULL == buffer) || (buffer_len == 0) || (NULL == tx_st_ptr))
	{
		return -1;
	}

	printf("START CHECK\n");

	//check 
	//partial label
	//partial version
	//get nversion
	//check segwit flag
	uint64_t counter = 0;
	SPC_TX tx_st;
	memset(&tx_st, '\0', sizeof(SPC_TX));
	tx_st.inputs_list = NULL;
	tx_st.outputs_list = NULL;
	tx_st.witnesses_list = NULL;
	int ret = -1;
	//partial label
	memcpy(tx_st.partial_label, buffer + counter, PARTIALLABELLEN);
	counter += PARTIALLABELLEN;
	uint8_t label_check [PARTIALLABELLEN] = {0x45, 0x50, 0x54, 0x46, 0xff};

#if 0
	for(int ii = 0; ii < PARTIALLABELLEN; ii ++)
	{
		printf("0x%02x ", tx_st.partial_label[ii]);
	}
	printf("\n");

	printf("cmp:%d\n", (strncmp(label_check, tx_st.partial_label, PARTIALLABELLEN)));
#endif

	if((memcmp(label_check, tx_st.partial_label, PARTIALLABELLEN)) != 0)
	{
		return -1;
	}
	
	//partial version
	memcpy(&(tx_st.partial_version), buffer + counter, PARTIALVERSIONLEN);
	counter += PARTIALVERSIONLEN;

	if(tx_st.partial_version != 0x00)
	{
		return -1;
	}

	printf("PARTIAL DONE\n");

	//nversion
	memcpy(&(tx_st.nversion), buffer + counter, sizeof(uint32_t));

	counter +=  sizeof(uint32_t);

	printf("NVERSION DONE\n");

	//check segwit flag
	uint8_t mark = 0;
	memcpy(&mark, buffer + counter, sizeof(uint8_t));
	counter += sizeof(uint8_t);

	if(mark != 0x00)
	{
		return -1;
	}

	memcpy(&(tx_st.segwit_flag), buffer + counter, sizeof(uint8_t) );
	counter += sizeof(uint8_t); 
	
	if(tx_st.segwit_flag != 0x01)
	{
		return -1;
	} 

	if(counter >= buffer_len)
	{
		return -1;
	}

	printf("SEGWIT FLAG DONE\n");


	//inputs init
	uint64_t inputs_len = 0;
	ret = init_inputs(buffer + counter, buffer_len - counter, &(tx_st.inputs_counter), &(tx_st.inputs_list), &inputs_len);
	
	if((ret != 0) && (tx_st.inputs_counter != 0) && (tx_st.inputs_list != NULL))
	{
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		return -1;	
	}

	counter += inputs_len;

	if(counter >= buffer_len)
	{
		//wrong data
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		return -1;
	}
	
	printf("INPUTS DONE\n");

	
	//outputs init
	uint64_t outputs_len = 0;
	ret = init_outputs(buffer + counter, buffer_len - counter, &(tx_st.outputs_counter), &(tx_st.outputs_list), &outputs_len);
	if((ret != 0) && (tx_st.outputs_counter != 0) && (tx_st.outputs_list != NULL))
	{
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		return -1;	
	}

	counter += outputs_len;

	if(counter >= buffer_len)
	{
		//wrong data
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		return -1;
	}
	
	printf("OUTPUTS DONE\n");

	
	//witnesses init
	uint64_t witnesses_len = 0;
	tx_st.witnesses_counter = tx_st.inputs_counter;
	ret = init_witnesses(buffer + counter, buffer_len - counter, &(tx_st.witnesses_counter), &(tx_st.witnesses_list), &witnesses_len);
	if((ret != 0) && (tx_st.witnesses_counter != 0) && (tx_st.witnesses_list != NULL))
	{
		free_witnesses(tx_st.witnesses_list, tx_st.witnesses_counter);
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		return -1;	
	}

	if(tx_st.witnesses_counter != tx_st.inputs_counter)
	{
		free_witnesses(tx_st.witnesses_list, tx_st.witnesses_counter);
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		return -1;	
	}
	counter += witnesses_len;

	if(counter >=  buffer_len)
	{
		//wrong data
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		free_witnesses(tx_st.witnesses_list, tx_st.witnesses_counter);
		return -1;
	}


	printf("WITNESSES DONE\n");


	//get lock_time
	memcpy(&(tx_st.lock_time), buffer + counter, sizeof(uint32_t));
	counter += sizeof(uint32_t);

	if(counter >  buffer_len)
	{
		//wrong data
		free_inputs(tx_st.inputs_list, tx_st.inputs_counter);
		free_outputs(tx_st.outputs_list, tx_st.outputs_counter);
		free_witnesses(tx_st.witnesses_list, tx_st.witnesses_counter);
		return -1;
	}

	printf("LOCK TIME DONE\n");


	//copy data to addr
	memcpy(tx_st_ptr, &tx_st, sizeof(SPC_TX));

	printf("COPY DONE\n");

	return 0;
}


void free_tx(SPC_TX *tx_st_ptr)
{
	//check input
	if(NULL == tx_st_ptr)
	{
		return;
	}

	printf("free input check\n");

	//free inputs
	if(NULL != tx_st_ptr->inputs_list)
	{
		free_inputs(tx_st_ptr->inputs_list, tx_st_ptr->inputs_counter);
	}

	printf("free free inputs done\n");

	//free outputs
	if(NULL != tx_st_ptr->outputs_list)
	{
		free_outputs(tx_st_ptr->outputs_list, tx_st_ptr->outputs_counter);
	}

	printf("free free outputs done\n");

	//free witnesses
	if(NULL != tx_st_ptr->witnesses_list)
	{
		free_witnesses(tx_st_ptr->witnesses_list, tx_st_ptr->witnesses_counter);
	}

	printf("free free witnesses done\n");


	return;
}





















#if 0

//test for deserialize
int main()
{
	//char to hex
	//uint8_t char_buffer[698] = 
	//"02000000000101804a870067a3d66f63c73605b2b6c67a674bc663f2dcfea541133b7edb99d72d0000000000fdffffff01daee0e000000000022002052ce50dcb48a7027aeee5267037165a14e4eefe6c5803148b30ac7e2cc9cb17a0400473044022031b33e661ed416055e97864528e426c0dbfc15a15d2bfe7357aa9a2e198cf7fa0220573e62d8e139657b869b4f44fec948883ba14d5df454f407ecc92dfde45662d701483045022100f5d4224b4db5be1f3d972641f3046a91dc48732ee926afc378b9b7518c7498e6022040c28e7f237665ce3278dc97cb8084f07d008e6fa6aebbe5d61f8677153f05e3016952210215734af25bc9e1cc5562b5ac519f194ad126bf9c3959e232c27278cacff2dc2221027921a2dc45023f3ae37e3503992ae11bd1243f0872dc25a9c27e74af1d60446e2103d840ee8d029e0da0bd5d1d186e03c12620d301e8d880d293ca3574af318834d153ae35b80900";
	//uint8_t char_buffer[2810] =	
	//"020000000930c4acb20b7793dc8a708d19a29b1e19c4c785f7ddf3522e1eaafbdf8e77b604460000006b48304502210097f7be7fcb7eae0e145ee52c951f174f287ac395fa5d4046cdd24459835431d3022034ae219014e04816d345f1436b9c69f0411f2ad8ecc3494a22ba4eb94e2fe118012103cd900bac6dcbc7704f02dd771eeeb123be5a79e301f7d83651146cb2b03f5e21feffffff4647788246abcff5168181d58a8a88fa0272ad22175e22abf2a93f20a4cdd3ae000000006a4730440220595083dcf346cfeb4cfa8a81df2b22b26cded3a4ff0e04d92c35eb87f87cd3d4022010fbefab9d4e5651c100d20f51deafd405a3f7f908582b73d7bce76633ff0a960121021d8e5ac784dea697199d1bb1dfd0c4ea85886bc0d1b74744397e1cf7588ae9defeffffff6267898427e0ce643ddf34b6f0b5c1d43cc1d94396b868d9a9a7a7382c642602000000006b4830450221008b939990e61c7f24ce13a0e8ffdfc51cec6eb7947916345347f6a228fdc96f720220605b925c835216c02dd8aeacd30e6d4abedc5d5c1ed801782186e32568bb0473012103977cd6c5f32713609471142c3b75f5b707155dde9acf519c0201c4a94fbe3f40feffffff7c3a17f90902e0f2223928edcd84926cfd37a212cc16416ce21c5f72a83041106c0700006b4830450221009ec1825f50eeeabf0882c416c079bd7116b25d91eeeb83ff709eb10e1c3f5fdd02201daa80d42e2f2a910fb7dbb6e6bffdc51654d78a82dfdb2d042a5c46cc85dabb0121023c7df3058f9a17becaa8f7608f14ef67fb9e077bbfd013d1b04ca274ea832c02feffffffa750bfc2a42ee3b12b5edea8c2b9e6dcda7803987f913b9c4502a468606ffd51050000006b483045022100d87b00209f902ff5e6bc509775b56e5041de6b08f5ef9eabcdf761031738ef48022016e41d11e9fce0870afebd53f21c1fcdd82ac528560a2a6da45c1da31f0d9b8d012103623ad5ca7b664d41c89d0f20a61ff40989afc56fb932478b15d381820340b063feffffffaeb0b904077370f4f34303536fd01cba926a13929dc74fa9e99e8eb63e3932fb010000006b483045022100b3ff4e39714db8669e30f1a6cff8ca30e88f8663a00b062ebb3a67593e91112802207a6efaaa5ec1df77ed6562cfe954d3ffe84199f1d97f6afd671fe3930e99ec64012103c2605f6106ea0a09a021ca5a10d25fd6cc8a03e9159052819feb428137e0519bfeffffffbbc35445ffc237eb38ec98c5afed0a562aa00770e077cb8bad454debdc5c0460000000006b48304502210092a5355f38543a9008f9acf2178622fbeead6dfcd0dcf3d9fa2f12854ff8f6840220679a7a04116c19d8e0c61bd985fafcbfbfaec4f808198141e9cff65aeec8f5a60121037cb3349987ff0c3c8594ddd0e1efe83eb2d7a485411f1c60b1f35aeef234c864feffffffc1d99f01325bdccbec53d304b0b6e38a653d76e943cf563b245c2b5480b7b1dd000000006a473044022024548927d1589c0a619a079609a7119aaeccfe52ffc104c7d7ee99a9116409e4022079df31876d482b7c5d11a8d3ca65402708700586ae692fa657b4032e76cf0351012102c711ac2beb7d4c9f5e4457fe6a01905f08f3a745f89320d6b53d1190e0fcff50feffffffebdaee2207b2fa11cc3ed2815a9b57bc45d9c067f45a4f048b22e308734741bb0d0000006a473044022053a3435f6da2e4bc3118216a8461afc7d395642ae99610442106fb2f39ae58b602202d50d829a2ffdeea691ee231df37f31bc0daba136e2895377823a2f7d6aaf195012102a362b0975d13db369765ddb31c76bfc8db974a50fd0e79a3e211b143853a4a0cfeffffff0282d4c211000000001976a914e8cea30989bd15530f819b766684b00dc7ba7cfa88accffe0e000000000017a914e9f5b14695e4f69cf3000c56a53224a61d2967fe8768840900";
	//uint8_t char_buffer[902] = 
	//"02000000000101804a870067a3d66f63c73605b2b6c67a674bc663f2dcfea541133b7edb99d72d0000000000fdffffff01daee0e000000000022002052ce50dcb48a7027aeee5267037165a14e4eefe6c5803148b30ac7e2cc9cb17afeffffffffaaf80e00000000000000050001ff483045022100f5d4224b4db5be1f3d972641f3046a91dc48732ee926afc378b9b7518c7498e6022040c28e7f237665ce3278dc97cb8084f07d008e6fa6aebbe5d61f8677153f05e30101fffd0201524c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189000000004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80000000004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0000000053ae35b80900";
	uint8_t char_buffer[1788] =
	"45505446ff000200000000010260263657f5afcabd8ba7a430e04495a752ad041d32b56d2af81891fe7702f3500000000000fdffffff60263657f5afcabd8ba7a430e04495a752ad041d32b56d2af81891fe7702f3500100000000fdffffff0285d60300000000002200208f252e1750dba55dd2b7cd55166ea04ff0f78ebd36bca625a69fab1e481347e160ae0a000000000022002052ce50dcb48a7027aeee5267037165a14e4eefe6c5803148b30ac7e2cc9cb17afeffffffffb82307000000000000000500473044022031f64277e6d4c6fa24aada61bb4c4cc23371e49976ae824531725b260c59f44102204e79cb816744e8290e7ae01610e49c9aca3eda826a311592014036d5646e0f810101ff01fffd0201524c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010000004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80010000004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0100000053aefeffffffff20a10700000000000000050001ff47304402202e162149363ec7e16f47bf08cbffbc19b6ad3e0102272419fbcf582a83e647b502204274cb878dbb1f973c3addc9ad492461913b37b95cc1f19b91a613f9caa403c20101fffd0201524c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80000002004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189000002004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0000020053aef5c10900";
	
	//uint8_t hex_buffer[698] = {'\0'};
	//uint8_t hex_buffer[2810] = {'\0'};
	//uint8_t hex_buffer[902] = {'\0'};
	uint8_t hex_buffer[1788] = {'\0'};

	int ret = -1;
	//ret = array_char_to_hex(902, char_buffer, 902, hex_buffer);
	//ret = array_char_to_hex(698, char_buffer, 698, hex_buffer);
	//ret = array_char_to_hex(2810, char_buffer, 2810, hex_buffer);
	ret = array_char_to_hex(1788, char_buffer, 1788, hex_buffer); 	
	if(ret < 0)
	{
		printf("array_char_to_hex():ret:%d\n", ret);
		return -1;
	}

	SPC_TX tx_st;
	memset(&tx_st, '\0', sizeof(SPC_TX));

	ret = init_tx(hex_buffer, 894, &tx_st);
	if(ret != 0)
	{
		printf("ret:%d\n", ret);
		printf("MAIN ERROR\n");
		return -1;
	}


	//deserialize inputs
	printf("init inputs:\n");
	printf("inputs_list:%p\n", tx_st.inputs_list);
	printf("inputs_counter:%lu\n", tx_st.inputs_counter);
	SPC_INPUT input_st;
	uint64_t i = 0;
	int j = 0;
	for(i = 0; i < tx_st.inputs_counter; i ++)
	{
		memcpy(&input_st, (tx_st.inputs_list) + i*sizeof(SPC_INPUT), sizeof(SPC_INPUT));
		printf("txin:\n");
		printf("prev_tx_hash:\n");
		for(j = 0; j < 32; j ++)
		{
			printf("%02x", input_st.prev_tx_hash[j]);
		}
		printf("\n");
		printf("prev_tx_index:\n");
		for(j = 0; j < 4; j ++)
		{
			printf("%02x", input_st.prev_tx_index[j]);
		}
		printf("\n");
		printf("txin_script_len:%lu\n", input_st.txin_script_len);
		printf("txin_script:\n");
		for(j = 0; j < input_st.txin_script_len; j ++)
		{
			printf("%02x", input_st.txin_script[j]);
		}
		printf("\n");
		printf("sequence_no:\n");
		for(j = 0; j < 4; j ++)
		{
			printf("%02x", input_st.sequence_no[j]);
		}
		printf("\n");
		printf("\n");
		printf("\n");
		
		
	}


	//deserialize outputs
	printf("init outputs:\n");
	printf("outputs_list:%p\n", tx_st.outputs_list);
	printf("outputs_counter:%lu\n", tx_st.outputs_counter);
	SPC_OUTPUT output_st;
	uint64_t value_int = 0;
	for(i = 0; i < tx_st.outputs_counter; i ++)
	{
		memcpy(&output_st, (tx_st.outputs_list) + i*sizeof(SPC_OUTPUT), sizeof(SPC_OUTPUT));
		printf("txout:\n");
		memcpy(&value_int, output_st.value, 8);
		printf("value:\n");
		printf("%lu\n", value_int);
		#if 0
		for(j = 0; j < 8; j ++)
		{
			printf("%02x", output_st.value[j]);
		}
		printf("\n");
		#endif		

		printf("txout_script_len:%lu\n", output_st.txout_script_len);
		printf("txout_script:\n");
		for(j = 0; j < output_st.txout_script_len; j ++)
		{
			printf("%02x", output_st.txout_script[j]);
		}
		printf("\n");
		printf("\n");
		printf("\n");
		
	}



	//deserialize witnesses
	printf("init witnesses:\n");
	printf("witnesses_list:%p\n", tx_st.witnesses_list);
	printf("witnesses_counter:%lu\n", tx_st.witnesses_counter);
	SPC_WITNESS witness_st;
	OP_ITEM	op_item_st;
	uint64_t jj;
	uint64_t k;
	for(i = 0; i < tx_st.witnesses_counter; i ++)
	{
		memcpy(&witness_st, (tx_st.witnesses_list) + i*sizeof(SPC_WITNESS), sizeof(SPC_WITNESS));
		printf("witness_value:%lu\n", witness_st.value);
		printf("witness_version:%d\n", witness_st.version);

		printf("op_items_num:%lu\n", witness_st.op_items_counter);

		for(jj = 0; jj < witness_st.op_items_counter; jj ++)
		{
			memcpy(&op_item_st, (witness_st.op_items_list) + jj*sizeof(OP_ITEM) , sizeof(OP_ITEM));
			printf("op_item_len:%lu\n", op_item_st.op_item_len);
			for(k = 0; k < op_item_st.op_item_len; k ++)
			{
				printf("%02x", op_item_st.op_item[k]);
			}
			printf("\n");
			printf("\n");	
		}
		printf("\n");
		printf("\n");
		printf("\n");
		
	}


	printf("lock_time:%d\n", tx_st.lock_time);
	for(jj = 0; jj < 4; jj ++)
	{
		printf("%02x", ((uint8_t*)(&(tx_st.lock_time)))[jj]);
	}

	printf("\n");
	free_tx(&tx_st);

	return 0;
}


#endif
