#ifndef SPCMODPREPARE_H
#define SPCMODPREPARE_H

#include "spcconfig.h"

#define KEEPLISTFILEPATH	SPCDATADIR "trash/keep.list"
  
#define SERVEREPUBDIR		SPCDATADIR "server/epub/"   

#define DATAFILETAIL	".data"
#define CHECKFILETAIL	".checksum"

#define TEMPDATAFILEPATH	SPCDATADIR "prepare/temp/temp.data"

#define TXFILENAMELEN	29

#define CMDFILENAMELEN	41

int _readDataToFile(int fd, uint32_t start, uint32_t length, char *filePath);

int _countDataFile(char *dirPath);

char *_findDataFile(char *dirPath, uint32_t *retFileNameLen);

void _freeDataFileName(char *pName);

//check md5
//#define RMCHECKFILECMD	"rm -f /mnt/data/spc/upload/*.checksum"
#define RMCHECKFILECMD	"rm -f " SPCDATADIR "upload/*.checksum"

int _getCheckFileName(char *dataFileName, uint32_t dataFileNameLen, char *checkFileName, uint32_t checkFileNameLen);

int _getMd5(char *checkFileName, uint32_t checkFileNameLen, char *md5Buf, uint32_t md5BufLen);

int _genMd5(char *dataFileName, uint32_t dataFileNameLen, char *md5Buf, uint32_t md5BufLen);

char *_getSig(char *checkFileName, uint32_t checkFileNameLen, uint32_t *sigLen);

void _freeSig(char *pSig);

int _checkSig(char *sig, uint32_t sigLen, char *md5, uint32_t md5Len);

void _rmCheckFile();

int _mvDataFileToTempDir(char *fileName, uint32_t fileNameLen);

int _checkTempDataInTempDir();

//check checksum


//check file_no
int _getFileNo(int fd, uint32_t *fileNo);

//check temp.data head and tail
int _checkHeadAndTail(int fd);

//generate keep.list
int _genKeepList(int fd);

//generate tx files
int _genTxFiles(int fd);

//generate user epub file
int _genCmdFiles(int fd);





int prepare_checkDataFile();

int prepare_processDataFile();









#endif
