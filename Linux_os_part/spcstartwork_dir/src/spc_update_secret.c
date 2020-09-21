#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>


#include "spcmodsecret.h"
#include "spcmodserver.h"
#include "spcmodhotp.h"
#include "spcmodseed.h"
#include "spcmodcrypt.h"
#include "spcmodinit.h"
#include "spcpem.h"
#include "spcconfig.h"


#define SECRETLEN	16
#define MAKEMAX		10

void spc_update_secret(uint32_t serverId, char *userEpub, uint32_t userEpubLen, char *uuid)
{
	int ret = -1;
	struct server_info info;
	memset(&info, '\0', sizeof(struct server_info));

	ret = server_getServerInfo(&info, serverId);
	if(ret != 0)
	{
		printf("Failed to get no.%d info.\n", serverId);
		return;
	}

	char secretBuf[SECRETLEN+1] = {'\0'};
	uint32_t secretSeed = time(NULL);

	ret = hotp_getNewSecret(secretBuf, SECRETLEN+1, secretSeed);
	if(ret != 0)
	{
		printf("Failed to generate new secret.\n");
		return;
	}

	memcpy(info.server_secret, secretBuf, SECRETLEN);

	//userepub to userepub.pem
	ret = buffer_to_pem(USEREPUBPEMFILEPATH, (uint8_t *)userEpub, userEpubLen);
	if(-1 == ret)
	{
		printf("Failed to gen user epub pem file.\n");
		return;
	}

	//gen sec file
	ret = secret_genSecFile(secretBuf, SECRETLEN, uuid, UUIDLEN, info.server_id);
	if(ret == -1)
	{
		printf("Failed to generate sec file.\n");
		return;
	}

	//BUG FIX:
	//clean counter to 1 after update OTP secret
	info.server_counter = 1;	

	ret = server_setServerInfo(&info);
	if(ret != 0)
	{
		printf("Failed to set server info into server.list.\n");
		//if failed to write into server.list should delete dir and struct in server.list 
		return;
	}	

	return;
}

