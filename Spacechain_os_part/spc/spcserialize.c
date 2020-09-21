#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#include "spcdeserialize.h"
#include "spctools.h"

///bottom
int get_single_input_hex_len(SPC_INPUT *input_st_ptr, uint64_t *input_len_ptr)
{
	//
	if((NULL == input_st_ptr) || (NULL == input_len_ptr))
	{
		return -1;
	}

	uint64_t counter = 0;
	//prehash point:32+4
	//sequence_no:4
	counter = 32 + 4 + 4;

	//get txin script length
	uint64_t txin_script_len = input_st_ptr->txin_script_len; 
	counter += txin_script_len;

	//get txin script length len
	uint8_t varint_len = 0;
	uint64_t varint_len_64 = 0;
	varint_len = gen_varint_len(txin_script_len);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	

	*input_len_ptr = counter;

	return 0;
}



///static
int get_inputs_hex_len(uint8_t *inputs_list, uint64_t inputs_counter, uint64_t *inputs_hex_len)
{
	//
	if((NULL == inputs_list) || (0 == inputs_counter) || (NULL == inputs_hex_len))
	{
		return -1;
	}

	//
	uint64_t counter = 0;

	//get txin counter
	//get txin counter varint len
	//add txin counter varint len
	uint8_t varint_len = 0;
	uint64_t varint_len_64 = 0;
	varint_len = gen_varint_len(inputs_counter);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	

	//for each get every txin len
	uint64_t single_input_len = 0;
	uint64_t i = 0;
	int ret = -1;
	for(i = 0; i < inputs_counter; i++)
	{
		single_input_len = 0;
		ret = get_single_input_hex_len((SPC_INPUT *)(inputs_list + i*sizeof(SPC_INPUT)), &single_input_len);
		if(-1 == ret)
		{
			return -1;
		}
		else
		{
			counter += single_input_len;
		}
	}
	
	//return counter
	*inputs_hex_len = counter;

	return 0;
}



///bottom
int get_single_output_hex_len(SPC_OUTPUT *output_st_ptr, uint64_t *output_len_ptr)
{
	//
	if((NULL == output_st_ptr) || (NULL == output_len_ptr))
	{
		return -1;
	}

	uint64_t counter = 0;
	//value:8
	counter = 8;

	//get txout script length
	uint64_t txout_script_len = output_st_ptr->txout_script_len; 
	counter += txout_script_len;

	//get txout script length len
	uint8_t varint_len = 0;
	uint64_t varint_len_64 = 0;
	varint_len = gen_varint_len(txout_script_len);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	

	*output_len_ptr = counter;

	return 0;
}



///static
int get_outputs_hex_len(uint8_t *outputs_list, uint64_t outputs_counter, uint64_t *outputs_hex_len)
{
	//
	if((NULL == outputs_list) || (0 == outputs_counter) || (NULL == outputs_hex_len))
	{
		return -1;
	}

	//
	uint64_t counter = 0;

	//get txout counter
	//get txout counter varint len
	//add txout counter varint len
	uint8_t varint_len = 0;
	uint64_t varint_len_64 = 0;
	varint_len = gen_varint_len(outputs_counter);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	

	//for each get every txout len
	uint64_t single_output_len = 0;
	uint64_t i = 0;
	int ret = -1;
	for(i = 0; i < outputs_counter; i++)
	{
		single_output_len = 0;
		ret = get_single_output_hex_len((SPC_OUTPUT *)(outputs_list + i*sizeof(SPC_OUTPUT)), &single_output_len);
		if(-1 == ret)
		{
			return -1;
		}
		else
		{
			counter += single_output_len;
		}
	}
	
	//return counter
	*outputs_hex_len = counter;

	return 0;
}


///bottom
int get_single_op_item_hex_len(OP_ITEM *op_item_st_ptr, uint64_t *op_item_len_ptr)
{
	//
	if((NULL == op_item_st_ptr) || (NULL == op_item_len_ptr))
	{
		return -1;
	}

	uint64_t counter = 0;

	//get op_item_len
	uint64_t op_item_len = op_item_st_ptr->op_item_len;
	counter += op_item_len;
	//get op_item_len varint len
	uint64_t varint_len_64 = 0;
	uint8_t varint_len = gen_varint_len(op_item_len);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	

	*op_item_len_ptr = counter;	
	
	return 0;
}

///
int get_single_witness_hex_len(SPC_WITNESS *witness_st_ptr, uint64_t *witness_len_ptr)
{
	//	
	if((NULL == witness_st_ptr) || (NULL == witness_len_ptr))
	{
		return -1;
	}

	uint64_t counter = 0;
	uint8_t varint_len = 0;
	uint64_t varint_len_64 = 0;
	uint64_t op_items_counter = 0;

	//op_items_counter varint_len
	op_items_counter = witness_st_ptr->op_items_counter;
	varint_len = gen_varint_len(op_items_counter);
	if(1 == varint_len)
	{	
		varint_len_64 = varint_len;
		counter += varint_len_64;
	}
	else if(3 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(5 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else if(9 == varint_len)
	{
		varint_len_64 = varint_len;
		counter += varint_len_64;

	}
	else
	{
		return -1;	
	}	
	
	//for each get every op_item len
	int ret = -1;
	uint64_t single_op_item_len = 0;
	uint64_t i = 0;
	for(i = 0; i < op_items_counter; i ++)
	{
		ret = get_single_op_item_hex_len((OP_ITEM *)(witness_st_ptr->op_items_list + i*sizeof(OP_ITEM)), &single_op_item_len);
		if(-1 == ret)
		{
			return -1;
		}
		else
		{
			counter += single_op_item_len;
		}
	}

	*witness_len_ptr = counter;
	return 0;
}


//
int get_witnesses_hex_len(uint8_t *witnesses_list, uint64_t witnesses_counter, uint64_t *witnesses_hex_len)
{
	//inputs
	if((NULL == witnesses_list) || (0 == witnesses_counter) || (NULL == witnesses_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;

	//for each get every witness hex len
	uint64_t single_witness_hex_len = 0;
	uint64_t i = 0;
	int ret = -1;
	for(i = 0; i < witnesses_counter; i ++)
	{
		ret = get_single_witness_hex_len((SPC_WITNESS *)(witnesses_list + i*sizeof(SPC_WITNESS)), &single_witness_hex_len);
		if(-1 == ret)
		{
			return -1;
		}
		else
		{
			counter += single_witness_hex_len;
		}
	}

	*witnesses_hex_len = counter;

	return 0;
}



int get_tx_hex_len(SPC_TX *tx_st_ptr, uint64_t *tx_hex_len)
{
	//
	if((NULL == tx_st_ptr) || (NULL == tx_hex_len))
	{
		return -1;
	}

	//version_no:	4
	//flag:		2
	//inputs_len:	
	//outputs_len:
	//witnesses_len:
	//lock_time:	4
	int ret = -1;	
	uint64_t counter = 0;
	uint64_t inputs_len = 0;
	uint64_t outputs_len = 0;
	uint64_t witnesses_len = 0;
	
	//version_no
	counter += 4;
	
	//flag
	counter += 2;
	
	//inputs
	ret = get_inputs_hex_len(tx_st_ptr->inputs_list, tx_st_ptr->inputs_counter, &inputs_len);
	if(0 == ret)
	{
		counter += inputs_len;
	}
	else
	{
		return -1;
	}

	//outputs
	ret = get_outputs_hex_len(tx_st_ptr->outputs_list, tx_st_ptr->outputs_counter, &outputs_len);
	if(0 == ret)
	{
		counter += outputs_len;
	}
	else
	{
		return -1;
	}

	//witnesses
	ret = get_witnesses_hex_len(tx_st_ptr->witnesses_list, tx_st_ptr->witnesses_counter, &witnesses_len);
	if(0 == ret)
	{
		counter += witnesses_len;
	}
	else
	{
		return -1;
	}

	//lock_time
	counter += 4;

	//return
	*tx_hex_len = counter;

	return 0;
}


//
int serialize_single_input(SPC_INPUT *input_st_ptr, uint8_t *buffer, uint64_t *input_hex_len)
{
	//
	if((NULL == input_st_ptr) || (NULL == buffer) || (NULL == input_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;	

	//prevtx hash 32
	memcpy(buffer + counter, input_st_ptr->prev_tx_hash, 32);
	counter += 32;

	//prevtx index 4
	memcpy(buffer + counter, input_st_ptr->prev_tx_index, 4);
	counter += 4;

	//txin script length(varint)
	int ret = -1;
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(input_st_ptr->txin_script_len);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}

	ret = gen_varint(input_st_ptr->txin_script_len, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}	

	memcpy(buffer + counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);
	
	//txin script
	size_t txin_script_len = input_st_ptr->txin_script_len;
	memcpy(buffer + counter, input_st_ptr->txin_script, txin_script_len);
	counter += txin_script_len;		

	//sequence
	memcpy(buffer + counter, input_st_ptr->sequence_no, 4);
	counter += 4;
	
	//return input hex len
	*input_hex_len = counter;

	return 0;
}


int serialize_inputs(uint8_t *inputs_list, uint64_t inputs_counter, uint8_t *buffer, uint64_t *inputs_hex_len)
{
	//inputs check
	if((NULL == inputs_list) || (0 == inputs_counter) || (NULL == buffer) || (NULL == inputs_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;

	//get inputs_counter
	//get inputs counter varint len
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(inputs_counter);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}
	//get inputs counter varint
	//copy varint
	int ret = -1;
	ret = gen_varint(inputs_counter, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}

	memcpy(buffer+counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);

	//for each serialize every input
	uint64_t i = 0;
	uint64_t single_input_len = 0;
	for(i = 0; i < inputs_counter; i ++)
	{
		single_input_len = 0;
		ret = serialize_single_input((SPC_INPUT *)(inputs_list + i*sizeof(SPC_INPUT)), buffer+counter, &single_input_len);
		if(0 == ret)
		{
			counter += single_input_len;
		}
		else
		{
			return -1;
		}
	
	}

	//return inputs hex len 
	*inputs_hex_len = counter;

	return 0;
}

//
int serialize_single_output(SPC_OUTPUT *output_st_ptr, uint8_t *buffer, uint64_t *output_hex_len)
{
	//
	if((NULL == output_st_ptr) || (NULL == buffer) || (NULL == output_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;	

	//value 8
	memcpy(buffer + counter, output_st_ptr->value, 8);
	counter += 8;

	//txout script length(varint)
	int ret = -1;
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(output_st_ptr->txout_script_len);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}

	ret = gen_varint(output_st_ptr->txout_script_len, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}	

	memcpy(buffer + counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);
	
	//txout script
	size_t txout_script_len = output_st_ptr->txout_script_len;
	memcpy(buffer + counter, output_st_ptr->txout_script, txout_script_len);
	counter += txout_script_len;		
	
	//return input hex len
	*output_hex_len = counter;

	return 0;
}


int serialize_outputs(uint8_t *outputs_list, uint64_t outputs_counter, uint8_t *buffer, uint64_t *outputs_hex_len)
{
	//inputs check
	if((NULL == outputs_list) || (0 == outputs_counter) || (NULL == buffer) || (NULL == outputs_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;

	//get outputs_counter
	//get outputs counter varint len
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(outputs_counter);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}
	//get outputs counter varint
	//copy varint
	int ret = -1;
	ret = gen_varint(outputs_counter, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}

	memcpy(buffer+counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);

	//for each serialize every output
	uint64_t i = 0;
	uint64_t single_output_len = 0;
	for(i = 0; i < outputs_counter; i ++)
	{
		single_output_len = 0;
		ret = serialize_single_output((SPC_OUTPUT *)(outputs_list + i*sizeof(SPC_OUTPUT)), buffer+counter, &single_output_len);
		if(0 == ret)
		{
			counter += single_output_len;
		}
		else
		{
			return -1;
		}
	
	}

	//return outputs hex len 
	*outputs_hex_len = counter;

	return 0;
}


//
int serialize_single_op_item(OP_ITEM *op_item_st_ptr, uint8_t *buffer, uint64_t *op_item_hex_len)
{
	//
	if((NULL == op_item_st_ptr) || (NULL == buffer) || (NULL == op_item_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;	

	//op_item length(varint)
	int ret = -1;
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(op_item_st_ptr->op_item_len);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}

	ret = gen_varint(op_item_st_ptr->op_item_len, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}	

	memcpy(buffer + counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);
	
	//op_item
	size_t op_item_len = op_item_st_ptr->op_item_len;
	memcpy(buffer + counter, op_item_st_ptr->op_item, op_item_len);
	counter += op_item_len;		
	
	//return op_item hex len
	*op_item_hex_len = counter;

	return 0;
}


int serialize_single_witness(SPC_WITNESS *witness_st_ptr, uint8_t *buffer, uint64_t *witness_hex_len)
{
	//inputs check
	if((NULL == witness_st_ptr) || (NULL == buffer) || (NULL == witness_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;
	uint8_t *op_items_list = witness_st_ptr->op_items_list;
	uint64_t op_items_counter = witness_st_ptr->op_items_counter;

	//get op_items_counter
	//get op items counter varint len
	uint8_t varint_len = 0;
	varint_len = gen_varint_len(op_items_counter);
	uint8_t *varint_ptr = NULL;
	size_t varint_len_size_t = varint_len;
	varint_ptr = malloc(varint_len_size_t);
	if(NULL == varint_ptr)
	{
		return -1;
	}
	//get op items counter varint
	//copy varint
	int ret = -1;
	ret = gen_varint(op_items_counter, varint_len, varint_ptr);
	if(-1 == ret)
	{
		free(varint_ptr);
		return -1;
	}

	memcpy(buffer+counter, varint_ptr, varint_len_size_t);
	counter += varint_len_size_t;
	free(varint_ptr);

	//for each serialize every op item
	uint64_t i = 0;
	uint64_t single_op_item_len = 0;
	for(i = 0; i < op_items_counter; i ++)
	{
		single_op_item_len = 0;
		ret = serialize_single_op_item((OP_ITEM *)(op_items_list + i*sizeof(OP_ITEM)), buffer+counter, &single_op_item_len);
		if(0 == ret)
		{
			counter += single_op_item_len;
		}
		else
		{
			return -1;
		}
	
	}

	//return witness hex len 
	*witness_hex_len = counter;

	return 0;
}


#if 1
int serialize_witnesses(uint8_t *witnesses_list, uint64_t witnesses_counter, uint8_t *buffer, uint64_t *witnesses_hex_len)
{
	//inputs check
	if((NULL == witnesses_list) || (0 == witnesses_counter) || (NULL == buffer) || (NULL == witnesses_hex_len))
	{
		return -1;
	}

	uint64_t counter = 0;

	//for each serialize every witness
	uint64_t i = 0;
	uint64_t single_witness_len = 0;
	int ret = -1;
	for(i = 0; i < witnesses_counter; i ++)
	{
		single_witness_len = 0;
		ret = serialize_single_witness((SPC_WITNESS *)(witnesses_list + i*sizeof(SPC_WITNESS)), buffer+counter, &single_witness_len);
		if(0 == ret)
		{
			counter += single_witness_len;
		}
		else
		{
			return -1;
		}
	
	}

	//return witness hex len 
	*witnesses_hex_len = counter;

	return 0;
}

#endif


int serialize_tx(SPC_TX *tx_st_ptr, uint8_t *buffer, uint64_t *tx_hex_len)
{
	//
	if((NULL == tx_st_ptr) || (NULL == buffer) || (NULL == tx_hex_len))
	{
		return -1;
	}

	//serialize
	uint64_t counter = 0;
	
	//version no
	memcpy(buffer+counter, &(tx_st_ptr->nversion), 4);
	counter += 4;
	
	//flag segwit
	//mark 0x00
	uint8_t mark = 0x00;
	memcpy(buffer+counter, &mark, 1);
	counter += 1;

	//flag 0x01
	memcpy(buffer+counter, &(tx_st_ptr->segwit_flag), 1);
	counter += 1;

	int ret = -1;
	//serialize inputs
	uint64_t inputs_hex_len = 0; 
	ret = serialize_inputs(tx_st_ptr->inputs_list, tx_st_ptr->inputs_counter, buffer+counter, &inputs_hex_len);
	if(-1 == ret)
	{
		return -1;
	}
	counter += inputs_hex_len;

	//serialize outputs
	uint64_t outputs_hex_len = 0;
	ret = serialize_outputs(tx_st_ptr->outputs_list, tx_st_ptr->outputs_counter, buffer+counter, &outputs_hex_len);
	if(-1 == ret)
	{
		return -1;
	}
	counter += outputs_hex_len;

	//serialize witnesses
	uint64_t witnesses_hex_len = 0;
	ret = serialize_witnesses(tx_st_ptr->witnesses_list, tx_st_ptr->witnesses_counter, buffer+counter, &witnesses_hex_len);
	if(-1 == ret)
	{
		return -1;
	}
	counter += witnesses_hex_len;

	//lock time
	memcpy(buffer+counter, &(tx_st_ptr->lock_time), 4);
	counter += 4;

	//return len
	*tx_hex_len = counter;

	return 0;

}

















