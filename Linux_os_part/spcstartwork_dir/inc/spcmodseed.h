#ifndef SPC_SEED_OPS_H
#define SPC_SEED_OSP_H

#include "spcconfig.h"

//
#define MAKEMAX			10	
#define CMDBUFLEN		200
#define SEEDPATHLEN		150
#define XPUBFILEPATHLEN		100
#define SEEDFILEPATHLEN		100
#define JSONBUFLEN		1024	
#define XKEYBUFLEN		130
#define XPUBSAVELEN		130
#define XPUBLEN			113	
#define SECRETLEN		16
#define IDLENGTHMAX		12

#define UUIDLEN			32

#define XPUBFILEDIR		SPCDATADIR "certifile/"

#define WALLETFILEDIR		SPCDATADIR "wallet/"

#define TMPWALLETFILEPATH	SPCDATADIR "wallet/wallet.tmp"

#define JSONTMPFILEPATH		SPCDATADIR "wallet/json.tmp"

#define DOWNLOADDIR		SPCDATADIR "download/"

int _saveXpubToFile(uint32_t serverId, uint16_t fileNo, uint16_t index, char *uuidBuf, uint8_t *xpub_buffer, uint32_t xpub_len);

int _checkStart(uint32_t serverId, uint16_t start);

int _mvToDownloadDir(uint32_t serverId, uint16_t fileNo, char *uuidBuf);

int seed_makeSeed(uint32_t serverId, uint16_t start, char *uuidBuf, uint32_t uuidLen);


#endif 
