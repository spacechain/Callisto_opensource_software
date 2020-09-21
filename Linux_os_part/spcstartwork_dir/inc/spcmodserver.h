#ifndef SPCMODSERVER_H
#define SPCMODSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "spcconfig.h"

//
#define IDLENGTHMAX	12

//path define
#define SERVERLISTPATH		SPCDATADIR "server/server.list"
#define WALLETDIRHEADER		SPCDATADIR "wallet/"
#define CERTIDIRHEADER		SPCDATADIR "certifile/"
#define TXFILEDIR		SPCDATADIR "transaction/txfiles/"
#define RESFILEDIR		SPCDATADIR "transaction/resfiles/"


//server info struct in server list file
struct server_info 	
{
	uint32_t server_id;
	uint32_t server_start;
	uint64_t server_txs;
	char server_secret[16];
	uint32_t server_counter;
};

//util
//
//if exists, return 0;
//else, return -1;
int _checkServerList(void);

//
//if succeed, return 0;
//if failed, return -1;
int _initServerList(void);

//
//if succeed, return 0;
//else, return -1;
int _getServerInfo(struct server_info *pInfo, uint32_t serverId);

//
//if succeed, return 0;
//else, return -1;
int _setServerInfo(struct server_info *pInfo, uint8_t isInfo0);

//
int _makeServerDir(uint32_t server_id);

//
void _printServerInfo(struct server_info *pInfo);

//server interface
//if succeed, return 0;
//else, return -1;
int server_getNewInfo(struct server_info *pInfo);

//
int server_initNewServer(struct server_info *pInfo);
//make dir
//set server info

//
int server_getServerInfo(struct server_info *pInfo, uint32_t serverId);

//
int server_setServerInfo(struct server_info *pInfo);

//
void server_showServerList(void);

#define UUIDLEN	32

#define USEREPUBFILETAIL	".userepub"

#define EPUBDIR		SPCDATADIR "server/epub/"

#define USEREPUBFILEPATH	SPCDATADIR "server/temp/user.epub"

int _countCmdFile(char *dirPath);

char *_getCmdFileName(char *dirPath, uint32_t *retNameLen);

void _freeRetFileName(char *pName);

int _mvCmdFileToTempDir(char *fileName, uint32_t fileNameLen);

int _parseUserEpubFile(char *uuidBuf, uint32_t uuidBufLen, uint32_t *serverId, char **userEpub, uint32_t *userEpubLen);

void _freeUserEpub(char *userEpub);

int server_countCmdFile();

int server_parseCmdFile(char *uuidBuf, uint32_t uuidBufLen, uint32_t *serverId, char **userEpub, uint32_t *userEpubLen);


#endif
