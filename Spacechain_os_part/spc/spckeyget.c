#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bip32.h"
#include "bip39.h"
#include "curves.h"
#include "ecdsa.h"
#include "memzero.h"
#include "spckeyget.h"

#define VERSION_PUBLIC 		0x0488b21e
#define VERSION_P2WSH_XPRV 	0x02aa7a99
#define VERSION_P2WSH_XPUB 	0x02aa7ed3

int get_pubkey_from_xpub_base58_double_ckd(uint8_t *xpub_base58, uint32_t xpub_len, uint32_t path1, uint32_t path2, uint8_t *buffer, uint32_t buffer_len)
{
	if((NULL == xpub_base58) || (xpub_len > 113) || (NULL == buffer) || (buffer_len < 33))
	{
		return -1;
	}

	//
	HDNode node_st;
	int ret = -1;
	uint32_t finger_print = 0;
	
	ret = hdnode_deserialize_public((const char *)xpub_base58, VERSION_P2WSH_XPUB, SECP256K1_NAME, &node_st, &finger_print);
	if(ret != 0)
	{
		printf("deserialize failed.\n");
		return -1;
	}	

	

	//first ckd
	ret = hdnode_public_ckd(&node_st, path1);
	if(ret != 1)
	{
		printf("first ckd failed.\n");
		return -1;
	}

	//second ckd
	ret = hdnode_public_ckd(&node_st, path2);
	if(ret != 1)
	{
		printf("second ckd failed.\n");
		return -1;
	}

	//copy pubkey
	memset(buffer, '\0', buffer_len);
	memcpy(buffer, node_st.public_key, 33);
	
	return 0;
}


int get_pubkey_from_xpub_double_ckd(uint8_t *xpub_non_base58, uint32_t xpub_len, uint32_t path1, uint32_t path2, uint8_t *buffer, uint32_t buffer_len)
{
	if((NULL == xpub_non_base58) || (xpub_len != 78) || (NULL == buffer) || (buffer_len < 33))
	{
		return -1;
	}

	//
	HDNode node_st;
	int ret = -1;
	uint32_t finger_print = 0;
	
	ret = hdnode_deserialize_public_non_base58(xpub_non_base58, 78, VERSION_P2WSH_XPUB, SECP256K1_NAME, &node_st, &finger_print);
	if(ret != 0)
	{
		printf("deserialize failed.\n");
		return -1;
	}	

	

	//first ckd
	ret = hdnode_public_ckd(&node_st, path1);
	if(ret != 1)
	{
		printf("first ckd failed.\n");
		return -1;
	}

	//second ckd
	ret = hdnode_public_ckd(&node_st, path2);
	if(ret != 1)
	{
		printf("second ckd failed.\n");
		return -1;
	}

	//copy pubkey
	memset(buffer, '\0', buffer_len);
	memcpy(buffer, node_st.public_key, 33);
	
	return 0;
}


//int get_privkey_from_xprv_double_ckd(uint8_t *xprv, uint32_t xprv_len, uint32_t path1, uint32_t path2, uint8_t *buffer, uint32_t buffer_len)
int get_node_from_xprv_double_ckd(uint8_t *xprv, uint32_t xprv_len, uint32_t path1, uint32_t path2, HDNode *node_ptr)
{
	if((NULL == xprv) || (xprv_len < 112) || (NULL == node_ptr))
	{
		return -1;
	}

	//
	HDNode node_st;
	int ret = -1;
	uint32_t finger_print = 0;
	
	ret = hdnode_deserialize_private((const char *)xprv, VERSION_P2WSH_XPRV, SECP256K1_NAME, &node_st, &finger_print);
	if(ret != 0)
	{
		printf("deserialize failed.\n");
		return -1;
	}	

	
	//first ckd
	ret = hdnode_private_ckd(&node_st, path1);
	if(ret != 1)
	{
		printf("first ckd failed.\n");
		return -1;
	}

	//second ckd
	ret = hdnode_private_ckd(&node_st, path2);
	if(ret != 1)
	{
		printf("second ckd failed.\n");
		return -1;
	}

	//copy pubkey
	//memset(buffer, '\0', buffer_len);
	//memcpy(buffer, node_st.private_key, 32);
	//copy node
	memcpy(node_ptr, &node_st, sizeof(HDNode));	

	return 0;
}


#if 0
int main(void)
{
#if 0
	uint8_t xpub_char[111] = 
	"Zpub6y6PVFa2DdTV5TYvJXcc761x3mXhP1AhtU6rJfW1SU1YQv1Cdd4yU3LqcRKpcKUBuDonMFLZEeHUqXEcoo5vZVvApr1JWdh3BMxr8EpD1md";

	uint8_t pub_buffer[34] = {'\0'};

	int ret = -1;
	ret = get_pubkey_from_xpub_double_ckd(xpub_char, 0, 2, pub_buffer, 34);
	if(ret != 0)
	{
		printf("MAIN ERROR\n");
		return -1;
	}

	int i = 0;
	for(i = 0; i < 33; i ++)
	{
		printf("%02x", pub_buffer[i]);
	}
	printf("\n");
#endif

	return 0;
}
#endif





