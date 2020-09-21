#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "spcscript.h"
#include "spctools.h"



int count_single_item(uint8_t *buffer, uint64_t *item_len)
{
	if((buffer == NULL) || (item_len == NULL))
	{
		*item_len = 0; 
		return -1;
	}

	uint8_t head = 0;
	uint64_t counter = 0; 
	memcpy(&head, buffer, sizeof(uint8_t));
	counter += sizeof(uint8_t);

	if((head >= 0x01) && (head <= 0x4b))
	{
		counter += head;
		*item_len = counter;
		return 0;
	}
	else if(head == OP_PUSHDATA1)
	{
		uint8_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint8_t));
		counter += sizeof(uint8_t);
		counter += size;
		*item_len = counter;
		return 0;

	}
	else if(head == OP_PUSHDATA2)
	{
		uint16_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint16_t));
		counter += sizeof(uint16_t);
		counter += size;
		*item_len = counter;
		return 0;

	}
	else if(head == OP_PUSHDATA4)
	{
		uint32_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint32_t));
		counter += sizeof(uint32_t);
		counter += size;
		*item_len = counter;
		return 0;

	}
	else
	{
		*item_len = counter;
		return 0;

	}
	
}



int get_single_item(uint8_t *buffer, SCRIPT_ITEM *script_item_st_ptr, uint64_t *item_len)
{
	if((buffer == NULL) || (script_item_st_ptr == NULL) || (item_len == NULL))
	{
		*item_len = 0; 
		return -1;
	}

	uint8_t head = 0;
	uint64_t counter = 0;
	SCRIPT_ITEM script_item_st; 
	memcpy(&head, buffer, sizeof(uint8_t));
	counter += sizeof(uint8_t);

	if((head >= 0x01) && (head <= 0x4b))
	{
		uint8_t *item_ptr = NULL;
		item_ptr = malloc(head);
		if(NULL == item_ptr)
		{
			return -1;
		}
		memcpy(item_ptr, buffer+counter, head);
		counter += head;
		script_item_st.item_len = head;
		script_item_st.item = item_ptr;
		memcpy(script_item_st_ptr, &script_item_st, sizeof(SCRIPT_ITEM));
		*item_len = counter;
		return 0;
	}
	else if(head == OP_PUSHDATA1)
	{
		uint8_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint8_t));
		counter += sizeof(uint8_t);
		if(size <= 0x4b)
		{
			return -1;
		}

		uint8_t *item_ptr = NULL;
		item_ptr = malloc(size);
		if(NULL == item_ptr)
		{
			return -1;
		}
		memcpy(item_ptr, buffer+counter, size);
		counter += size;
		script_item_st.item_len = size;
		script_item_st.item = item_ptr;
		memcpy(script_item_st_ptr, &script_item_st, sizeof(SCRIPT_ITEM));
		*item_len = counter;
		return 0;

	}
	else if(head == OP_PUSHDATA2)
	{
		uint16_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint16_t));
		counter += sizeof(uint16_t);
		uint8_t *item_ptr = NULL;
		item_ptr = malloc(size);
		if(NULL == item_ptr)
		{
			return -1;
		}
		memcpy(item_ptr, buffer+counter, size);
		counter += size;
		script_item_st.item_len = size;
		script_item_st.item = item_ptr;
		memcpy(script_item_st_ptr, &script_item_st, sizeof(SCRIPT_ITEM));
		*item_len = counter;
		return 0;



	}
	else if(head == OP_PUSHDATA4)
	{
		uint32_t size = 0;
		memcpy(&size, buffer+counter, sizeof(uint32_t));
		counter += sizeof(uint32_t);
		uint8_t *item_ptr = NULL;
		item_ptr = malloc(size);
		if(NULL == item_ptr)
		{
			return -1;
		}
		memcpy(item_ptr, buffer+counter, size);
		counter += size;
		script_item_st.item_len = size;
		script_item_st.item = item_ptr;
		memcpy(script_item_st_ptr, &script_item_st, sizeof(SCRIPT_ITEM));
		*item_len = counter;
		return 0;


	}
	else
	{
		uint8_t *item_ptr = NULL;
		item_ptr = malloc(sizeof(uint8_t));
		if(NULL == item_ptr)
		{
			return -1;
		}
		memcpy(item_ptr, &head, sizeof(uint8_t));
		script_item_st.item_len = 0;
		script_item_st.item = item_ptr;
		memcpy(script_item_st_ptr, &script_item_st, sizeof(SCRIPT_ITEM));
		*item_len = counter;
		return 0;

	}
	
}


int parse_script(uint8_t *buffer, uint64_t script_len, SPC_SCRIPT *script_st_ptr)
{
	if((NULL == buffer) || (script_len == 0) || (NULL == script_st_ptr))
	{
		return -1;
	}

	uint64_t sum = 0;
	uint64_t counter = 0;
	SCRIPT_ITEM script_item_st ;
	uint64_t items_num = 0;
	int ret = -1; 
	while(sum < script_len)
	{
		ret = count_single_item(buffer+sum, &counter);	
		if(ret != 0)
		{
			return -1;
		}
		sum += counter;
		items_num ++;
	}

	if(sum != script_len)
	{
		return -1;
	}

	sum = 0;
	counter = 0;
	uint8_t *items_list = NULL;
	items_list = malloc(items_num*sizeof(SCRIPT_ITEM));
	if(NULL == items_list)
	{
		return -1;
	}

	for(uint64_t i = 0; i < items_num; i ++)
	{
		ret = get_single_item(buffer+sum, &script_item_st, &counter);
		if(ret != 0)
		{
			return -1;
		}
		
		//copy to 
		memcpy(items_list + i*sizeof(SCRIPT_ITEM), &script_item_st, sizeof(SCRIPT_ITEM));
		sum += counter;

	}

	if(sum != script_len)
	{
		return -1;

	}

	SPC_SCRIPT spc_script_st;
	spc_script_st.items_num = items_num;
	spc_script_st.items_list = items_list;
	memcpy(script_st_ptr, &spc_script_st, sizeof(SPC_SCRIPT));

	return 0;
}

void free_script(SPC_SCRIPT *spc_script_st_ptr)
{
	if(NULL == spc_script_st_ptr)
	{
		return;
	}
	
	uint64_t i = 0;
	SCRIPT_ITEM script_item_st;
	for(i = 0; i < (spc_script_st_ptr->items_num); i++)
	{
		memcpy(&script_item_st, (spc_script_st_ptr->items_list)+i*sizeof(SCRIPT_ITEM), sizeof(SCRIPT_ITEM));
		free(script_item_st.item);
	}

	free(spc_script_st_ptr->items_list);
	spc_script_st_ptr->items_list = NULL;	
	return;
}

#if 0
//test main
int main()
{
	uint8_t script_char[516] = 
	"524c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189000000004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80000000004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0000000053ae";	
	uint8_t script_hex[516] = {'\0'};	
	//uint8_t script_char[210] = 
	//"52210215734af25bc9e1cc5562b5ac519f194ad126bf9c3959e232c27278cacff2dc2221027921a2dc45023f3ae37e3503992ae11bd1243f0872dc25a9c27e74af1d60446e2103d840ee8d029e0da0bd5d1d186e03c12620d301e8d880d293ca3574af318834d153ae";
	//uint8_t script_hex[210] = {'\0'};

	int ret = -1;
	//ret = array_char_to_hex(210, script_char, 210, script_hex);
	ret = array_char_to_hex(516, script_char, 516, script_hex);
	
	SPC_SCRIPT witness_script_st;
	SCRIPT_ITEM script_item_st;
	ret = parse_script(script_hex, 258, &witness_script_st);
	printf("ret:%d\n", ret);

	//show
	uint64_t i;
	uint32_t j;
	for(i = 0; i < witness_script_st.items_num; i ++)
	{
		memcpy(&script_item_st, (witness_script_st.items_list) + i*sizeof(SCRIPT_ITEM), sizeof(SCRIPT_ITEM));
		printf("item_len:%d\n", script_item_st.item_len);
		if(script_item_st.item_len == 0)
		{
			printf("op_code:0x%02x\n", *(uint8_t *)(script_item_st.item));
		}
		else
		{
			for(j = 0; j < script_item_st.item_len; j ++)
			{
				printf("%02x", script_item_st.item[j]);
			}
			printf("\n");	
		}	

	}
	

	

	free_script(&witness_script_st);

	


	return 0;
}

#endif


