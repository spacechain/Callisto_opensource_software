#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "cotp.h"
#include "spcmodhotp.h"
#include "spc_new_base32.h"


//#include <openssl/evp.h>
//#include <openssl/hmac.h>

#define RAWHOTPLEN	6
#define SECRETLEN	16
#define HOTPLEN		16
	


//
int hotp_getNewSecret(char *dest, uint32_t len, uint32_t seed)
{
	//check
	if((NULL == dest) || (len <= SECRETLEN))
	{
		printf("hotp_getNewSecret():");
		printf("Make sure dest buffer valid and len more than 16 bytes.\n");
		return -1;
	}	

	return spc_new_base32(dest, len, seed);

}


//
void _paddingHotp(char *dest)
{
	int i;
	for(i = RAWHOTPLEN; i < HOTPLEN; i++)
	{
		dest[i] = '0';
	}

	dest[HOTPLEN] = '\0';

	return;
}


//
int hotp_genHotp(char *secret, uint32_t counter, char *dest, uint32_t len) 
{
	//check
	if((NULL == secret) || (0 == counter) || (NULL == dest) || (len <= HOTPLEN))
	{
		printf("hotp_genHotp():");
		printf("Got invalid input.\n");
		return -1;
	}

	cotp_error_t err;
	int64_t counter_long = counter;
	//printf("counter_long=%ld\n", counter_long);
	char *hotp = get_hotp(secret, counter_long, 6, SHA1, &err);
	if(NULL == hotp)
	{
		printf("hotp_genHotp():");
		printf("Failed to generate hotp.\n");
		return -1;
	}
	//printf("hotp:%s\n", hotp);
	memcpy(dest, hotp, 6);
	free(hotp);
	_paddingHotp(dest);	

	return 0;
}


//
int hotp_genNHotp(char *secret, uint32_t counter, uint32_t times, char *dest, uint32_t len)
{
	//check 
	if((NULL == secret) || (0 == counter) || (0 == times) || (NULL == dest) || (len < times*(HOTPLEN+1)))
	{
		printf("hotp_genNHotp():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//
	int ret = -1;
	for(uint32_t index = 0 ; index < times; index ++)
	{
		ret = hotp_genHotp(secret, counter+index, dest+(index*(HOTPLEN+1)), HOTPLEN+1);
		if(ret == -1)
		{
			printf("hotp_genNHotp():hotp_genHotp():");
			printf("Failed to generate hotp no.%d.\n", index+1);
			return -1;
		}

	}

	return 0;
}

