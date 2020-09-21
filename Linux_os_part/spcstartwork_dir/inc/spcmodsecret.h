#ifndef SPCMODSECRET_H
#define SPCMODSECRET_H
#include <stdio.h>
#include <string.h>
#include "spcconfig.h"

#define SECRETTEMPDIR	SPCDATADIR "secret/temp/"

#define DOWNLOADDIR	SPCDATADIR "download/"

#define TEMPSECFILEPATH	SPCDATADIR "secret/temp/temp.sec"

#define UUIDLEN	32

int _writeDmsgToTempFile(char *uuidBuf, uint32_t uuidBufLen, uint32_t serverId, char *dMsg, uint32_t dMsgLen);

int _checkTempSec();

int _mvTempSecToDownloadDir(char *uuidBuf, uint32_t uuidBufLen, uint32_t serverId);

int secret_genSecFile(char *secretBuf, uint32_t secretLen, \
		      char *uuidBuf, uint32_t uuidBufLen, \
	              uint32_t serverId);



#endif
