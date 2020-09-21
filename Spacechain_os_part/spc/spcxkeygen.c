#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bip32.h"
#include "bip39.h"
#include "curves.h"
#include "ecdsa.h"
#include "memzero.h"

#define VERSION_PUBLIC 		0x0488b21e
#define VERSION_P2WSH_XPRV 	0x02aa7a99
#define VERSION_P2WSH_XPUB 	0x02aa7ed3
//strength = 256
int spc_seed_gen(uint8_t *seed, int seed_len)
{
	//
	if((seed == NULL) || (seed_len != 64))
	{
		printf("ERROR:Check seed buf now!\n");
		return -1;
	}

	const char *mnemonic_ptr = NULL;
	mnemonic_ptr = mnemonic_generate(256);
	int i = 0;
	int j = 0;
	for(i = 0; i < 24; i++)
	{
		for(j = 0; j < 10; j++)
		{
			printf("%c", mnemonic_ptr[i*10+j]);
		}
		//printf(" ");
	}
	printf("\n");

	mnemonic_to_seed(mnemonic_ptr, "spacechain", seed, NULL);

	return 0;
}


int spc_hdnode_gen_from_seed(uint8_t *seed, int seed_len, void *node_ptr)
{
	//
	if((NULL == seed) || (64 != seed_len) || (NULL == node_ptr))
	{
		printf("ERROR:spc_hdnode_gen_from_seed():check input.\n");
		return -1;
	}

	int ret = -1;
	ret = hdnode_from_seed(seed, 64, SECP256K1_NAME, node_ptr);
	if(ret != 1)
	{
		printf("ERROR:Failed to generate root node from seed.\n");
		return -1;
	}
	return 0;
}


int spc_node_from_rootnode(void *root_node_ptr, void *node_ptr, uint32_t *fingerprint)
{
	//
	if((NULL == root_node_ptr) || (NULL == node_ptr))
	{
		printf("ERROR:spc_node_from_rootnode():check input.\n");
		return -1;
	}

	int ret = -1;
	memcpy(node_ptr, root_node_ptr, sizeof(HDNode));
	uint32_t fp = 0;
	fp = hdnode_fingerprint(root_node_ptr);
	memcpy(fingerprint, &fp, sizeof(uint32_t));
	
	ret = hdnode_private_ckd_prime(node_ptr, 1);
	if(ret != 1)
	{
		printf("ERROR:Failed to ckd_prime node from root node.\n");
		return -1;
	}

	return 0;
}


int spc_serialize_private(void *node_ptr, char *xprv_buf, int xprv_buf_len, uint32_t fingerprint, uint32_t version)
{
	//
	if((NULL == node_ptr) || (NULL == xprv_buf) || (xprv_buf_len < 112) || (fingerprint == 0) || (version == 0))
	{
		printf("spc_serialize_private():check input.\n");
		return -1;
	}

	int ret = -1;

	ret = hdnode_serialize_private(node_ptr, fingerprint, version, xprv_buf, xprv_buf_len);	
	if(ret == 0)
	{
		printf("ERROR:Failed to serialize xprv.\n");
		return -1;
	}
	printf("spc_serialize_private():ret:%d\n", ret);

	return ret;
}

int spc_serialize_public(void *node_ptr, char *xpub_buf, int xpub_buf_len, uint32_t fingerprint, uint32_t version)
{
	//
	if((NULL == node_ptr) || (NULL == xpub_buf) || (xpub_buf_len < 112) || (fingerprint == 0) || (version == 0))
	{
		printf("spc_serialize_public():check input.\n");
		return -1;
	}

	int ret = -1;
	hdnode_fill_public_key(node_ptr);
	ret = hdnode_serialize_public(node_ptr, fingerprint, version, xpub_buf, xpub_buf_len);
	if(ret == 0)
	{
		printf("ERROR:Failed to serialize xpub.\n");
		return -1;
	}

	printf("spc_serialize_public():ret:%d\n", ret);

	return ret;
}


//
#if 1
int create_single_key_pair(uint8_t *xpub_buffer, uint32_t xpub_buffer_len, uint8_t *xprv_buffer, uint32_t xprv_buffer_len) 
{
	//inputs check
	if((NULL == xpub_buffer) || (xpub_buffer_len < 112) || (NULL == xprv_buffer) || (xprv_buffer_len < 112))
	{
		return -1;
	}

	//gen seed
	uint8_t seed[64] = {'\0'};
	int ret = -1;
	ret = spc_seed_gen(seed, 64);
	if(ret < 0)
	{
		//printf("create_single_key_pair():Failed to generate seed, %d.\n", ret);
		return -1;
	}

#if 0
	int i = 0;
	for(i = 0; i < 64; i++)
	{
		printf("%02x ", seed[i]);
	}
	printf("\n");
#endif


	//from seed to root node
	HDNode root_node, node;
	uint32_t fingerprint = 0;
	memzero(&root_node, sizeof(root_node));	
	memzero(&node, sizeof(node));	
	
	ret = spc_hdnode_gen_from_seed(seed, 64, &root_node);
	if(ret < 0)
	{
		//printf("create_single_key_pair():Failed to generate hdnode from seed, %d.\n", ret);
		return -1;
	}
	

	//from rootnode to node
	ret = spc_node_from_rootnode(&root_node, &node, &fingerprint);
	if(ret < 0)
	{
		//printf("create_single_key_pair():Failed to ckd node from rootnode, %d.\n", ret);
		return -1;
	}

	printf("depth:%d\n", node.depth);
	
	char xprv[113] = {'\0'};
	char xpub[113] = {'\0'};
		
//VERSION_P2WSH_XPRV 	0x02aa7a99
//VERSION_P2WSH_XPUB 	0x02aa7ed3

	ret = spc_serialize_private(&node, xprv, 112, fingerprint, VERSION_P2WSH_XPRV);
	if(ret < 0)
	{
		printf("SPCXPUBGEN:Failed to serialize private key, %d.\n", ret);
		return -1;
	}
	//print	
	printf("create_single_key_pair():xprv:%s, %d.\n", xprv, ret);
	if(ret > 112)
	{
		printf("create_single_key_pair():get big xprv:ret:%d.\n", ret);
		return -1;
	}
	else
	{
		memcpy(xprv_buffer, xprv, ret);
	}

	ret = spc_serialize_public(&node, xpub, 112, fingerprint, VERSION_P2WSH_XPUB);
	if(ret < 0)
	{
		printf("SPCXPUBGEN:Failed to serialize public key, %d.\n", ret);
		return -1;
	}
	//print		
	printf("create_single_key_pair():xpub:%s, %d.\n", xpub, ret);
	if(ret > 112)
	{
		printf("create_single_key_pair():get big xpub:ret:%d.\n", ret);
		return -1;
	}
	else
	{
		memcpy(xpub_buffer, xpub, ret);
	}

	return 0;
}



#if 0
int main(void)
{
	uint8_t xpub[113] = {'\0'};
	uint8_t xprv[113] = {'\0'};
	int ret = -1;
	ret = create_single_key_pair(xpub, 113, xprv, 113);
	printf("test main():ret:%d.\n", ret);
	printf("xpub:\n");
	for(ret = 0; ret < 113; ret ++)
	{
		printf("%c", xpub[ret]);
	}
	printf("\n");
		
	printf("xprv:\n");
	for(ret = 0; ret < 113; ret ++)
	{
		printf("%c", xprv[ret]);
	}
	printf("\n");
		

	return 0;
}

#endif 


#endif
