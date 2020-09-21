#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include "spcmodsecret.h"
#include "spcrsa.h"

#define READSTEP		50	
#define ENCRYPTEDMSGMAX		512	 

int _writeDmsgToTempFile(char *uuidBuf, uint32_t uuidBufLen, uint32_t serverId, char *dMsg, uint32_t dMsgLen)
{
	//check 
	if((NULL == uuidBuf) || (uuidBufLen < UUIDLEN) || (0 == serverId) || (NULL == dMsg) || (0 == dMsgLen))
	{
		printf("_writeDmsgToTempFile():");
		printf("GOt invalid inputs.\n");
		return -1;
	}

	//open
	int fd = -1;
	fd = open(TEMPSECFILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd < 0)
	{
		perror("_writeDmsgToTempFile():open():");
		printf("Failed to create and open temp/temp.sec.\n");
		return -1;
	}


	//write 
	int ret = -1;
	ret = write(fd, uuidBuf, UUIDLEN);
	if(UUIDLEN != ret)
	{
		if(-1 == ret)
		{
			perror("_writeDmsgToTempFile():write():");
		}
		else
		{
			printf("_writeDmsgToTempFile():write():");
		}
		printf("Failed to write uuid to temp.sec.\n");
		close(fd);
		return -1;
	}

	ret = write(fd, &serverId, sizeof(serverId));
	if(sizeof(serverId) != ret)
	{
		if(-1 == ret)
		{
			perror("_writeDmsgToTempFile():write():");
		}
		else
		{
			printf("_writeDmsgToTempFile():write():");
		}
		printf("Failed to write serverId to temp.sec.\n");
		close(fd);
		return -1;
	}

	ret = write(fd, &dMsgLen, sizeof(dMsgLen));
	if(sizeof(dMsgLen) != ret)
	{
		if(-1 == ret)
		{
			perror("_writeDmsgToTempFile():write():");
		}
		else
		{
			printf("_writeDmsgToTempFile():write():");
		}
		printf("Failed to write dmsg length to temp.sec.\n");
		close(fd);
		return -1;
	}

	ret = write(fd, dMsg, dMsgLen);
	if(dMsgLen != ret)
	{
		if(-1 == ret)
		{
			perror("_writeDmsgToTempFile():write():");
		}
		else
		{
			printf("_writeDmsgToTempFile():write():");
		}
		printf("Failed to write dMsg to temp.sec.\n");
		close(fd);
		return -1;
	}

	//close	
	close(fd);

	return 0;
}


int _checkTempSec()
{
	return(access(TEMPSECFILEPATH, R_OK));
}


int _mvTempSecToDownloadDir(char *uuidBuf, uint32_t uuidBufLen, uint32_t serverId)
{
	//check
	if((NULL == uuidBuf) || (uuidBufLen < UUIDLEN) || (0 == serverId))
	{
		printf("_mvTempSecToDownloadDir():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//gen path
	char *pathBuf = NULL;
	pathBuf = malloc(strlen(DOWNLOADDIR)+5+1+UUIDLEN+4+1);
	if(NULL == pathBuf)
	{
		printf("_writeDmsgToTempFile():malloc():");
		printf("Failed to allocate memory for buffer.\n");
		return -1;
	}
	memset(pathBuf, '\0', (strlen(DOWNLOADDIR)+5+1+UUIDLEN+4+1));

	strncat(pathBuf, DOWNLOADDIR, strlen(DOWNLOADDIR));
	sprintf(pathBuf+strlen(DOWNLOADDIR), "%05d-", serverId);
	memcpy(pathBuf+strlen(DOWNLOADDIR)+6, uuidBuf, UUIDLEN);
	strncat(pathBuf, ".sec", strlen(".sec"));
	printf("%s\n", pathBuf);


	//rename
	int ret = -1;
	ret = rename(TEMPSECFILEPATH, pathBuf);
	if(-1 == ret)
	{
		perror("_writeDmsgToTempFile():rename()");
		printf("Failed to move temp.sec to downlaod directory.\n");
		free(pathBuf);
		return -1;
	}
	free(pathBuf);

	return 0;
}


int secret_genSecFile(char *secretBuf, uint32_t secretBufLen, \
		      char *uuidBuf, uint32_t uuidBufLen, \
		      uint32_t serverId)
{
	int ret = -1;
	uint8_t *retBuf1 = NULL;
	uint32_t retLen1 = 0;
	uint8_t *retBuf2 = NULL;
	uint32_t retLen2 = 0;
	
	ret = user_en_enc_and_en64((uint8_t *)secretBuf, secretBufLen, &retBuf1, &retLen1);
	if(-1 == ret)
	{
		printf("secret_genSecFile():");
		printf("Failed to encrypt secret with user pub.\n");
		return -1;
	}

	ret = spc_en_enc_and_en64((uint8_t *)retBuf1, retLen1, &retBuf2, &retLen2);
	if(-1 == ret)
	{
		printf("secret_genSecFile():");
		printf("Failed to encrypt secret.user with spc pub.\n");
		free(retBuf1);
		return -1;
		
	}	

	free(retBuf1);

	ret = _writeDmsgToTempFile(uuidBuf, uuidBufLen, serverId, retBuf2, retLen2);
	if(-1 == ret)
	{
		printf("secret_genSecFile():");
		printf("Failed to encrypted secret to temp.sec.\n");
		free(retBuf2);
		return -1;
	}
	free(retBuf2);

	ret = _checkTempSec();
	if(-1 == ret)
	{
		printf("secret_genSecFile():");
		printf("Temp.sec not found.\n");
		return -1;
	}

	ret = _mvTempSecToDownloadDir(uuidBuf, uuidBufLen, serverId);
	if(-1 == ret)
	{
		printf("secret_genSecFile():");
		printf("Failed to move temp.sec to ID-UUID.sec.\n");
		return -1;
	}

	return 0;
}

