#include <stdio.h>
#include <string.h>

#include "spctools.h"
#include "sha2.h"

void sha256d(uint8_t *in, uint32_t in_len, uint8_t *hash, uint32_t hash_len)
{
	if((NULL == in) || (in_len == 0) || (NULL == hash) || (hash_len < 32))
	{
		return;
	}

	uint8_t hash_once[32] = {'\0'};
	uint8_t hash_twice[32] = {'\0'};
	size_t length = 0;

	length = in_len;
	sha256_Raw(in, length, hash_once);

	length = 32;
	sha256_Raw(hash_once, length, hash_twice);

	memset(hash, '\0', 32);
	memcpy(hash, hash_twice, 32);
	
	return;

}


#if 0
int main(void)
{
	uint8_t in_char[144] = 
	"fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f00000000ef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a01000000";
	
	uint8_t in_hex[144] = {'\0'};

	int ret = -1;
	ret = array_char_to_hex(144, in_char, 144, in_hex);
	if(ret == -1)
	{
		printf("array char to hex failed.\n");
		return -1;
	}

	uint8_t hash[32] = {'\0'};
	sha256d(in_hex, 72, hash, 32);

	int i;
	for(i = 0; i < 32; i++)
	{
		printf("%02x", hash[i]);
	}
	printf("\n");

	return 0;
}

#endif
