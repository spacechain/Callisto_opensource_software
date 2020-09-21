#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

//
#include "spcmodseed.h"
#include "spctcpc.h"

//
int _saveXpubToFile(uint32_t serverId, uint16_t fileNo, uint16_t index, char *uuid, uint8_t *xpub_buffer, uint32_t xpub_len)
{
	FILE *xFd;
	int len;
	size_t nBytes;

	if((fileNo >= 0) && ((fileNo % MAKEMAX) == 0) && (index >= fileNo) && (index < (fileNo+MAKEMAX)))
	{
	} 
	else
	{
		printf("_saveXpubToFile(): ");
		printf("Get a bad index.\n");
		return -1;
	}
	
#if 0
	nBytes = sprintf(xpubSaveBuf, "%03d", index);
	if(nBytes != 3)
	{
		perror("_spcSlaveSaveXpubToFile():sprintf()");
		printf("Failed to write index to buffer.\n");
		return -3;
	}
#endif

	char xpubFilePath[XPUBFILEPATHLEN] = {'\0'};
	memset(xpubFilePath, '\0', XPUBFILEPATHLEN);
	//write header
	strncat(xpubFilePath, XPUBFILEDIR, strlen(XPUBFILEDIR));
	
	//write id
	char idBuf[IDLENGTHMAX] = {'\0'};
	sprintf(idBuf, "%d", serverId);
	strncat(idBuf, "/", strlen("/"));

	strncat(xpubFilePath, idBuf, strlen(idBuf));
	sprintf(xpubFilePath+strlen(xpubFilePath), "%05d-", serverId);

	//write fileNo
	char fileNoBuf[IDLENGTHMAX] = {'\0'};
	memset(fileNoBuf, '\0', IDLENGTHMAX);
	sprintf(fileNoBuf, "%05d.ce", fileNo);
	strncat(xpubFilePath, fileNoBuf, strlen(fileNoBuf));

	int wRet = 0;
	printf("%s\n", xpubFilePath);
	if(access(xpubFilePath, F_OK) == -1)
	{
		printf("DEBUG\n");
		xFd = fopen(xpubFilePath, "w+");
		if(NULL == xFd)
		{
			perror("_saveXpubToFile():fopen()");	
			printf("Failed to open xpub file.\n");
			return -4;
		}
	
		wRet = fwrite(uuid, 1, UUIDLEN, xFd);
		if(wRet < UUIDLEN)
		{
			printf("_saveXpubToFile():fwrite():");
			printf("Failed to write uuid to file.\n");
			fclose(xFd);
			return -4;
		}	
		wRet = fwrite(&serverId, 1, sizeof(uint32_t), xFd);
		if(wRet < sizeof(uint32_t))
		{
			printf("_saveXpubToFile():fwrite():");
			printf("Failed to write serverId to file.\n");
			fclose(xFd);
			return -4;
		}	
		fseek(xFd, sizeof(uint32_t)+UUIDLEN, SEEK_SET);
	}
	else
	{
		xFd = fopen(xpubFilePath, "a");
		if(NULL == xFd)
		{
			perror("_saveXpubToFile():fopen()");	
			printf("Failed to open xpub file.\n");
			return -4;
		}
	}

	//write index
	nBytes = fwrite(&index, 1, sizeof(uint16_t), xFd);
	if(nBytes != (sizeof(uint16_t)))
	{
		perror("_saveXpubToFile():fwrite()");
		printf("Failed to write index to file.\n");
		fclose(xFd);
		return -4;
	}	

	//write length
	uint32_t xpub_len_num = 113;
	nBytes = fwrite(&xpub_len_num, 1, sizeof(uint32_t), xFd);
	if(nBytes != (sizeof(uint32_t)))
	{
		perror("_saveXpubToFile():fwrite()");
		printf("Failed to write length to file.\n");
		fclose(xFd);
		return -4;
	}	

	//write xpub
	nBytes = fwrite(xpub_buffer, 1, 113, xFd);
	if(nBytes != 113)
	{
		perror("_saveXpubToFile():fwrite()");
		printf("Failed to write xpub to file.\n");
		fclose(xFd);
		return -4;
	}	
	
	fclose(xFd);

	return 0;
}


//
int _checkStart(uint32_t serverId, uint16_t start)
{
	//size_t nBytes = 0;

	char xpubFilePath[XPUBFILEPATHLEN] = {'\0'};
	//write header
	strncat(xpubFilePath, XPUBFILEDIR, strlen(XPUBFILEDIR));
		
	//write id
	char idBuf[IDLENGTHMAX] = {'\0'};
	sprintf(idBuf, "%d", serverId);
	strncat(idBuf, "/", strlen("/"));
	strncat(xpubFilePath, idBuf, strlen(idBuf));

	//write start 
	char startBuf[IDLENGTHMAX] = {'\0'};
	sprintf(startBuf, "%d", start);
	strncat(xpubFilePath, startBuf, strlen(startBuf));	

	if(access(xpubFilePath, F_OK) == 0)
	{
		return -1;
	}	
	else
	{
		return 0;
	}

}


int _mvToDownloadDir(uint32_t serverId, uint16_t fileNo, char *uuidBuf)
{
	//gen path to sat pub path
	char idBuf[IDLENGTHMAX] = {"\0"};
	memset(idBuf, '\0', IDLENGTHMAX);
	sprintf(idBuf, "%d", serverId);
	strncat(idBuf, "/", strlen("/"));
	char xpubFilePath[XPUBFILEPATHLEN];
	memset(xpubFilePath, '\0', XPUBFILEPATHLEN);
	//write header
	strncat(xpubFilePath, XPUBFILEDIR, strlen(XPUBFILEDIR));
	
	//write id
	strncat(xpubFilePath, idBuf, strlen(idBuf));
	sprintf(xpubFilePath+strlen(xpubFilePath), "%05d-", serverId);

	//write fileNo
	char fileNoBuf[IDLENGTHMAX] = {'\0'};
	memset(fileNoBuf, '\0', IDLENGTHMAX);
	sprintf(fileNoBuf, "%05d.ce", fileNo);
	strncat(xpubFilePath, fileNoBuf, strlen(fileNoBuf));

	//gen new path
	char newPath[XPUBFILEPATHLEN];
	memset(newPath, '\0', XPUBFILEPATHLEN);
	strncat(newPath, DOWNLOADDIR, strlen(DOWNLOADDIR));
	sprintf(newPath + strlen(DOWNLOADDIR), "%05d-", serverId);
	strncat(newPath, uuidBuf, UUIDLEN);
	strncat(newPath, ".satpub", strlen(".satpub"));

	//rename
	int ret = -1;
	ret = rename(xpubFilePath, newPath);
	if(-1 == ret)
	{
		perror("_mvToDownloadDir()");
		printf("Failed to move sat pub file to downlaod directory.\n");
		return -1;
	}

	return 0;

}



//
int seed_makeSeed(uint32_t serverId, uint16_t start, char *uuidBuf, uint32_t uuidBufLen)
{
	uint16_t index = 0;
	int ret = -1;

	if((start < 0) || ((start % MAKEMAX) != 0))
	{
		printf("seed_makeSeed():");
		printf("Get a bad start.\n");
		return -1;
	}

#if 0
	ret = _checkStart(serverId, start);
	if(ret < 0)
	{
		printf("seed_makeSeed():_checkStart():");
		printf("xpub file existed.\n");
		return -1;
	}	
#endif

	if((NULL == uuidBuf) || (uuidBufLen < UUIDLEN))
	{
		printf("seed_makeSeed():");
		printf("invalid uuid buf.\n");
		return -1;
	}

	uint8_t tcp_xpub[113] = {'\0'};
	uint16_t tcp_index = 0;

	for(index = start; index < start+MAKEMAX; index++)
	{
		memset(tcp_xpub, '\0', 113);
		tcp_index = 0;

		//gen key pair
		printf("gen_new_keypair():id:%d\n", serverId);
		ret = tcp_gen_new_keypair(serverId, &tcp_index, tcp_xpub);
		if(-1 == ret)
		{
			printf("seed_makeSeed():");
			printf("failed to gen new keypair.\n");
			return -1;
		}
	
		if(tcp_index != index)
		{
			printf("seed_makeSeed():");
			printf("failed to match index.\n");
			return -1;
		}

		//save to file
		ret = _saveXpubToFile(serverId, start, index, uuidBuf, tcp_xpub, 113);
		if(ret < 0)
		{
			printf("seed_makeSeed():_saveXpubToFile():");
			printf("Failed to save Xpub No.%d to file.\n", index);
		}

		printf("seed_makeSeed():");
		printf("Generate seed No.%d\n", index);	

	}

	ret = _mvToDownloadDir(serverId, start, uuidBuf);
	if(-1 == ret)
	{
		printf("seed_makeSeed():");
		printf("Failed to move sat pub file to download directory.\n");
		return -1;
	}

	return 0;
}


//
void _getSeedPath(char *path, int len, uint32_t serverId, uint16_t index)
{
	//clear buf
	memset(path, '\0', len);

	//strcat dir path
	strncat(path, WALLETFILEDIR, strlen(WALLETFILEDIR));

	//strcat id path
	char idBuf[IDLENGTHMAX] = {'\0'};
	sprintf(idBuf, "%d", serverId);
	strncat(idBuf, "/", strlen("/"));
	strncat(path, idBuf, strlen(idBuf));
	
	//strcat index
	char indexBuf[IDLENGTHMAX] = {'\0'};
	sprintf(indexBuf, "%d", index);
	strncat(path, indexBuf, strlen(indexBuf));

	//print path
	printf("_spcSlaveGetSeedPath():");
	printf("Get one path:%s\n", path);
	
	return;
}


//
void spcSlaveRemoveSeed(uint32_t serverId, uint16_t start)
{
	//
	if((start<0) || ((start % MAKEMAX) != 0))
	{
		printf("seed_removeSeed():");
		printf("Get a bad start.\n");
		return;
	}

	int index = 0;
	int ret = -1;	

	//new buf
	char path[SEEDPATHLEN] = {'\0'};

	for(index = start; index < start+10; index++)
	{
		//call
		_getSeedPath(path, SEEDPATHLEN, serverId, index);
		
		//remove
		ret = remove(path);	
		if(ret < 0)
		{
			printf("seed_removeSeed():");
			printf("Failed to remove seed No.%d\n", index);	
			continue;
		}

		//return index ack 
		//after redesign
		printf("seed_removeSeed():");
		printf("Remove seed No.%d\n", index);	
	}
	
	return;
}

