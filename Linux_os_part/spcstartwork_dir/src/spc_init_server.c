#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "spcmodserver.h"
#include "spcmodhotp.h"
//#include "spcmodcrypt.h"
#include "spcmodinit.h"
#include "spcmodsecret.h"
#include "spcmodseed.h"
#include "spctcpc.h"

#include "spc_init_server.h"
#include "spcpem.h"
#include "spcconfig.h"

#define SECRETLEN	16
#define MAKEMAX		10

void spc_init_server(char *uuid, char *userEpub, uint32_t userEpubLen)
{
	int ret = -1;
	struct server_info info;
	memset(&info, '\0', sizeof(struct server_info));

	ret = server_getNewInfo(&info);
	if(ret != 0)
	{
		printf("Failed to get new info.\n");
		return;
	}

	ret = tcp_gen_new_keystore(info.server_id);
	if(ret == -1)
	{
		printf("Failed to gen new keystore for new server.\n");
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

	printf("DEBUG:GEN NEW SECRET DONE\n");

	ret = server_initNewServer(&info);
	if(ret != 0)
	{
		printf("Failed to init new server.\n");
		return;
	}

	printf("DEBUG:NEW SERVER INIT DONE\n");
	
	ret = seed_makeSeed(info.server_id, info.server_start, uuid, UUIDLEN);
	if(ret != 0)
	{
		printf("Failed to make seed for new server.\n");
		return;
	}

	info.server_start += MAKEMAX;
	
	//userepub to userpub.pem
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

	ret = server_setServerInfo(&info);
	if(ret != 0)
	{
		printf("Failed to set server info into server.list.\n");
		//if failed to write into server.list should delete dir and struct in server.list 
		return;
	}	
	return;
}

