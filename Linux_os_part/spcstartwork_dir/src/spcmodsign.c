#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <net/if.h>
#include <error.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <stdint.h>

#include "spcmodsign.h"
#include "spctcpc.h"


//free yourself
//get tx hex
int  _parse_data(uint8_t *buf_in, uint32_t buf_in_len, uint16_t *index, uint8_t **hex, uint32_t *hex_len)
{
	//
	if((NULL == buf_in) || (0 == buf_in_len) || (NULL == index) || (NULL == hex) || (NULL == hex_len))
	{
		printf("data input check failed.\n");
		return -1;	
	}	

	uint32_t counter = 0;

	//length
	//check tail
	uint8_t head = 0;
	uint8_t tail = 0;
	head = buf_in[0];
	if(0xa1 != head)
	{
		printf("data in wrong head.\n");
		return -1;
	}
	counter += 1;

	uint16_t total_len = 0;
	memcpy(&total_len, buf_in+counter, sizeof(uint16_t));
	counter += sizeof(uint16_t);
	
	tail = buf_in[counter+total_len-1];
	if(0x1a != tail)
	{
		printf("data in wrong tail.\n");
		return -1;
	} 	

	//index
	uint16_t get_index = 0;
	head = buf_in[counter];
	if(0xba != head)
	{
		printf("failed to get 0xba head.\n");
		return -1;
	}
	counter += 1;
	counter += sizeof(uint16_t);
	memcpy(&get_index, buf_in+counter, sizeof(uint16_t));
	counter += sizeof(uint16_t);
	*index = get_index;

	//hex
	head = buf_in[counter];
	if(0xca != head)
	{
		printf("failed to get 0xca head.\n");
		return -1;
	}
	counter += 1;
	
	uint16_t get_hex_len = 0;
	uint32_t get_hex_len_32 = 0;
	memcpy(&get_hex_len, buf_in+counter, sizeof(uint16_t));
	if(get_hex_len >= total_len)
	{
		printf("get wrong hex length.\n");
		return -1;
	}
	counter += sizeof(uint16_t);
	get_hex_len_32 = get_hex_len;
	*hex_len = get_hex_len_32;

	uint8_t *get_hex_ptr = NULL;
	get_hex_ptr = malloc(get_hex_len_32);
	if(NULL == get_hex_ptr)
	{
		printf("failed to malloc for hex.\n");
		return -1;
	}
	memset(get_hex_ptr, '\0', get_hex_len_32);

	memcpy(get_hex_ptr, buf_in+counter, get_hex_len_32);
	counter += get_hex_len_32;

	*hex = get_hex_ptr;
	return 0;
}



//sign

//gen res data
int _gen_res_data(uint8_t *res_hex, uint32_t res_hex_len, uint8_t *res_buf, uint32_t *res_data_len)
{
	//
	if((NULL == res_hex) || (0 == res_hex_len) || (NULL == res_buf) || (NULL == res_data_len))
	{
		return -1;
	}

	uint32_t counter = 0;
	//head
	res_buf[0] = 0xc1;
	counter += 1;
	
	//length
	uint16_t hex_len_16 = res_hex_len;
	memcpy(res_buf+counter, &hex_len_16, sizeof(uint16_t));
	counter += 2;

	//hex
	memcpy(res_buf+counter, res_hex, res_hex_len);
	counter += res_hex_len;

	//tail
	uint8_t tail = 0x1c;
	memcpy(res_buf+counter, &tail, 1);
	counter += 1;

	*res_data_len = counter;

	return 0;
}



//			
int sign_signTransaction(unsigned char *inBuf, uint32_t inLen, unsigned char *outBuf, uint32_t outLen, uint32_t *pDataLen, uint32_t serverId)
{
	//
	if((NULL == inBuf) || (0 == inLen) || (NULL == outBuf) || (NULL == pDataLen))
	{
		return -1;
	}

	//get index, tx_hex, tx_hex_len
	uint16_t index = 0;
	uint8_t *tx_hex = NULL;
	uint32_t tx_hex_len = 0;
	int ret = -1;
	ret = _parse_data((uint8_t *)inBuf, inLen, &index, &tx_hex, &tx_hex_len);
	if(-1 == ret)
	{
		printf("failed to parse data.\n");
		return -1;
	}

	uint32_t i;
	printf("tx_hex:\n");
	for(i = 0; i < tx_hex_len; i ++)
	{
		printf("%02x", tx_hex[i]);
	}
	printf("\n");


	//sign
	uint64_t tx_hex_len_64 = 0;
	tx_hex_len_64 = tx_hex_len;
	uint64_t res_hex_len_64 = 0;
	uint32_t res_hex_len_32 = 0;
	uint8_t *res_hex = NULL;

	ret = tcp_sign_tx(serverId, index, tx_hex, tx_hex_len_64, &res_hex, &res_hex_len_64);
	if(-1 == ret)
	{
		printf("failed to sign tx.\n");
		free(tx_hex);
		return -1;
	}
	free(tx_hex);
	res_hex_len_32 = res_hex_len_64;

	//gen res data	
	uint32_t res_data_len = 0;	
	ret = _gen_res_data(res_hex, res_hex_len_32, (uint8_t *)outBuf, &res_data_len);
	if(-1 == ret)
	{
		printf("failed to gen res data.\n");
		free(res_hex);
		return -1;
	}

	*pDataLen = res_data_len;
	return 0;

}



