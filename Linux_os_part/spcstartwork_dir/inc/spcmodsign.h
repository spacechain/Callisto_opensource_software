#ifndef SPCMODSIGN_H
#define SPCMODSIGN_H
#include <stdio.h>
#include "spcconfig.h"

#define SIGNXPUBLEN		107
#define IDLENGTHMAX		12	
#define MAKEMAX			10	
#define CMDBUFLEN		200
#define SEEDPATHLEN		150
#define XPUBFILEPATHLEN		100
#define SEEDFILEPATHLEN		100
#define JSONBUFLEN		1024	
#define XKEYBUFLEN		130
#define XPUBSAVELEN		130

#define XPUBFILEDIR		SPCDATADIR "certifile/"

#define WALLETFILEDIR		SPCDATADIR "wallet/"

#define TMPWALLETFILEPATH	SPCDATADIR "wallet/wallet.tmp"

#define XPUBHEADER		"Zpub"


typedef struct {
	uint8_t 	ucStartFlag;
	uint16_t 	ucDataLong;
	uint8_t		ucData[1];
} __attribute__((packed)) ProtocolDataHead;



int sign_signTransaction(unsigned char *inBuf, uint32_t inLen, unsigned char *outBuf, uint32_t outLen, uint32_t *pDataLen, uint32_t serverId);



#endif
