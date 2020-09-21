#ifndef SPCMODINIT_H
#define SPCMODINIT_H
#include <stdint.h>
#include "spcconfig.h"

#define TEMPEPUBFILEPATH	SPCDATADIR "node/temp/epub.temp"

#define EPUBFILEPATH	SPCDATADIR "node/epub.file"

#define SPCEPUBFILETAIL	".spcepub"

#define NODETEMPDIR	SPCDATADIR "node/temp/"

#define SPCEPUBFILETEMPPATH	SPCDATADIR "node/temp/spc.epub"

#define UUIDLEN	32	

#define TEMPSATEPUBFILEPATH	SPCDATADIR "node/temp/temp.satepub"

#define SATEPUBFILETAIL	".satepub"

//exchagnge pubkey

//sat epub
int _saveSatEpubToPemFile(char *satEpubBuf, uint32_t satEpubLen);

//spc epub
char *_findSpcEpubFile(char *dirPath, uint32_t *retNameLen);

void _freeSpcEpubFileName(char *pName);

int _countSpcEpubFile(char *dirPath);

int _mvSpcEpubFileToTempDir(char *fileName, uint32_t fileNameLen);

int _checkSpcEpubFileInTempDir(void);

int _parseSpcEpubFile(char *uuidBuf, uint32_t uuidBufLen, char **ppSpcEpub, uint32_t *pSpcEpubLen);

void _freeSpcEpub(char *pSpcEpub);

//epub.file
int _saveSpcEpubToPemFile(char *spcEpubBuf, uint32_t spcEpubLen);

//gen xxxx.satepub
int _genTempSatEpubFile(char *uuid, uint32_t uuidLen, char *satEpub, uint32_t satEpubLen);

int _genUuidSatEpubFile(char *uuid, uint32_t uuidLen);

int init_checkSpcEpubFile();

int init_exchangeEpub(void);


//check stage 1
#define SPCCS1FILETAIL	".spccs1"

#define SPCCS1FILEPATH	SPCDATADIR "node/temp/spc.cs1"

#define SECRET	"issnode"

#define MSPCDATAFILEPATH	SPCDATADIR "node/mspc.data"

#define TEMPSATCS1FILEPATH	SPCDATADIR "node/temp/temp.satcs1"

#define SATCS1FILETAIL	".satcs1"

int _countUuidSpcCs1File(char *dirPath);

char *_findUuidSpcCs1File(char *dirPath, uint32_t *pFileNameLen);

void _freeUuidSpcCs1FileName(char *pFileName);

int _mvUuidSpcCs1ToSpcCs1(char *fileName, uint32_t fileNameLen);

int _checkSpcCs1(void);

char *_getSpcEpub(uint32_t *pSpcEpubLen);

char * init_getSpcEpub(uint32_t *pSpcEpubLen);

void _freeSpcEpub(char *pSpcEpub);

int _saveMspcToFile(char *mSpc, uint32_t mSpcLen);//M.spc

int _parseSpcCs1(char *uuidBuf, uint32_t uuidBufLen);//get uuid and Msat_up

int _genTempSatCs1(char *uuid, uint32_t uuidLen, char *mSpc, uint32_t mSpcLen);//temp/temp.satcs1

int _genUuidSatCs1(char *uuid, uint32_t uuidLen);

int init_checkSpcCs1();

int init_cs1();

//check stage 2
#define SPCCS2FILETAIL	".spccs2"

#define SPCCS2FILEPATH	SPCDATADIR "node/temp/spc.cs2"

#define THEWORDS	"spc"

#define TEMPSATCS2FILEPATH	SPCDATADIR "node/temp/temp.satcs2"

#define SATCS2FILETAIL	".satcs2"

int _countUuidSpcCs2File(char *dirPath);

char *_findUuidSpcCs2File(char *dirPath, uint32_t *pFileNameLen);

void _freeUuidSpcCs2FileName(char *pFileName);

int _mvUuidSpcCs2ToSpcCs2(char *fileName, uint32_t fileNameLen);

int _checkSpcCs2();

//get Msat(spc.cs1 spc.cs2)
char *_getMsat(uint32_t *pMsatLen);

void _freeMsat(char *pMsat);

char *_getSatEpub(uint32_t *pSatEpubLen);

void _freeSatEpub(char *pSatEpub);

int _checkMsg(char *msg, uint32_t msgLen);

int _parseSpcCs2(char *uuidBuf, uint32_t uuidBufLen);//get uuid and Msat_up

char *_getMspc(uint32_t *mSpcLen);

int _genTempSatCs2(char *uuid, uint32_t uuidLen, char *mSpc, uint32_t mSpcLen);//temp/temp.satcs2 if mSpc == NULL

int _genUuidSatCs2(char *uuid, uint32_t uuidLen);

int init_checkSpcCs2();

int init_cs2();
























#endif 
