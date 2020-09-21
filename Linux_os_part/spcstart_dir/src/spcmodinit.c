#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "spcrsa.h"
#include "spcpem.h"
#include "spcmodnode.h"
#include "spcmodinit.h"
#include "spcconfig.h"


#define READSTEP	10	

//pubkeys exchange
char *_findSpcEpubFile(char *dirPath, uint32_t *retNameLen)
{
	//check inputs
	if((NULL == dirPath) || (NULL == retNameLen))
	{
		printf("_findSpcEpubFile():");
		printf("Got invalid inputs.\n");
		*retNameLen = 0;
		return NULL;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		perror("_findSpcEpubFile():opendir()");
		printf("Failed to open upload directory.\n");
		*retNameLen = 0;
		return NULL;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	void *pFileName = NULL;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCEPUBFILETAIL)), SPCEPUBFILETAIL, strlen(SPCEPUBFILETAIL)) == 0)
		{
			pFileName = malloc(strlen(pD_st->d_name)+1);
			if(NULL == pFileName)
			{
				printf("_findSpcEpubFile():readdir():malloc():");
				printf("Failed to allocate memory for ret name buffer.\n");

				if(-1 == closedir(dUl))
				{
					perror("_findSpcEpubFile():readdir():closedir()");
					printf("Failed to excute closedir().\n");
				}
				
				*retNameLen = 0;
				return NULL;
			}

			memset(pFileName, '\0', strlen(pD_st->d_name)+1);
			memcpy(pFileName, (pD_st->d_name), strlen(pD_st->d_name));
			break;			
		}	
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		perror("_findSpcEpubFile():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		free(pFileName);
		*retNameLen = 0;
		return NULL;
	}
	 
	//return
	if(NULL == pFileName)
	{
		*retNameLen = 0;
		return NULL;
	}

	*retNameLen = strlen(pD_st->d_name);
	return pFileName; 

}


void _freeSpcEpubFileName(char *pName)
{
	free(pName);
	return;
}


int _countSpcEpubFile(char *dirPath)
{
	//check inputs
	if((NULL == dirPath))
	{
		printf("_countSpcEpubFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		perror("_countSpcEpubFile():opendir()");
		printf("Failed to open upload directory.\n");
		return -1;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	int retCount = 0;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCEPUBFILETAIL)), SPCEPUBFILETAIL, strlen(SPCEPUBFILETAIL)) == 0)
		{
			retCount++;
		}	
		
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		perror("_countSpcEpubFile():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		return -1;
	}
	 
	//return
	return retCount; 

}


int _mvSpcEpubFileToTempDir(char *fileName, uint32_t fileNameLen)
{
	//check 
	if((NULL == fileName) || (0 == fileNameLen))
	{
		printf("_mvSpcEpubFileToTempDir():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//gen old path and new path
	void *pOldPath = NULL;
	pOldPath = malloc(strlen(DEPLOYDIR)+fileNameLen+1); 
	if(NULL == pOldPath)
	{	
		printf("_mvSpcEpubFileToTempDir():");
		printf("Failed to allocate memory for spc epub file old path.\n");
		return -1;
	}
	memset(pOldPath, '\0', strlen(DEPLOYDIR)+fileNameLen+1);
	memcpy(pOldPath, DEPLOYDIR, strlen(DEPLOYDIR));
	strncat(pOldPath, fileName, fileNameLen);
	printf("OLDPATH:%s.\n", (char *)pOldPath);

	void *pNewPath = NULL;
	pNewPath = malloc(strlen(SPCEPUBFILETEMPPATH)+1); 
	if(NULL == pNewPath)
	{	
		printf("_mvSpcEpubFileToTempDir():");
		printf("Failed to allocate memory for spc epub file new path.\n");
		free(pOldPath);
		return -1;
	}
	memset(pNewPath, '\0', strlen(SPCEPUBFILETEMPPATH)+1);
	memcpy(pNewPath, SPCEPUBFILETEMPPATH, strlen(SPCEPUBFILETEMPPATH));
	printf("NEWPATH:%s.\n", (char *)pNewPath);

	//move
	if(-1 == rename(pOldPath, pNewPath))
	{
		perror("_mvSpcEpubFileToTempDir()");
		printf("Failed to move spc epub file from upload directory to temp directory.\n");
		free(pOldPath);
		free(pNewPath);
		return -1;
	}
	
	free(pOldPath);
	free(pNewPath);
	return 0;
}


int _checkSpcEpubFileInTempDir()
{
	return (access(SPCEPUBFILETEMPPATH, R_OK));	
}


int _parseSpcEpubFile(char *uuidBuf, uint32_t uuidBufLen, char **ppSpcEpub, uint32_t *pSpcEpubLen)
{
	//check input
	if((NULL == uuidBuf) || (uuidBufLen < 32) || (NULL == ppSpcEpub) || (NULL == pSpcEpubLen))
	{
		printf("_parseSpcEpubFile():");
		printf("Got invalid inputs.\n");
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;
	}

	//open file
	int fd = -1;
	fd = open(SPCEPUBFILETEMPPATH, O_RDONLY);
	if(fd < 0)
	{
		perror("_parseSpcEpubFile():open()");
		printf("Failed to open spc.epub in temp dir.\n");
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
	}
	
	
	//read uuid
	int ret = -1;
	memset(uuidBuf, '\0', UUIDLEN);
	ret = read(fd, uuidBuf, UUIDLEN);
	if(ret != UUIDLEN)
	{
		if(-1 == ret)
		{
			perror("_parseSpcEpubFile()");
		}
		else
		{
			printf("_parseSpcEpubFile():");
		}
		printf("Failed to read uuid from spc.epub.\n");
		close(fd);
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
			
	}

	//read spc pubkey len
	uint32_t spcEpubLen = 0;
	ret = read(fd, &spcEpubLen, sizeof(spcEpubLen));
	if(ret != sizeof(spcEpubLen))
	{
		if(-1 == ret)
		{
			perror("_parseSpcEpubFile()");
		}
		else
		{
			printf("_parseSpcEpubFile():");
		}
		printf("Failed to read spc epub length from spc.epub.\n");
		close(fd);
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
			
	}

	if(spcEpubLen > 2048)
	{
		printf("_parseSpcEpubFile():");
		printf("Failed to read spc epub length from spc.epub.\n");
		close(fd);
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
	}
	
	//read spc pubkey
	void *pSpcEpub = NULL;
	pSpcEpub = malloc(spcEpubLen);
	if(NULL == pSpcEpub)
	{
		printf("_parseSpcEpubFile():");
		printf("Failed to allocate memory for spc epub buffer.\n");
		close(fd);
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
		
	}
	memset(pSpcEpub, '\0', spcEpubLen);

	ret = read(fd, pSpcEpub, spcEpubLen);
	if(ret != spcEpubLen)
	{
		if(-1 == ret)
		{
			perror("_parseSpcEpubFile()");
		}
		else
		{
			printf("_parseSpcEpubFile():");
		}
		printf("Failed to read spc epub from spc.epub.\n");
		close(fd);
		free(pSpcEpub);
		*ppSpcEpub = NULL;
		*pSpcEpubLen = 0;
		return -1;	
			
	}

	close(fd);

	//return
	*ppSpcEpub = pSpcEpub;
	*pSpcEpubLen = spcEpubLen;
	return 0;
}


void _freeSpcEpub(char *pSpcEpub)
{
	free(pSpcEpub);
	return;
}


int _saveSpcEpubToPemFile(char *spcEpub, uint32_t spcEpubLen)
{
	//check input
	if((NULL == spcEpub) || (0 == spcEpubLen))
	{
		printf("_saveSpcEpubToFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	int ret = -1;
	ret = buffer_to_pem(SPCEPUBPEMFILEPATH, spcEpub, spcEpubLen);
	if(-1 == ret)
	{
		return -1;
	}

	return 0;
}

int _saveSatEpubToTempFile(char *satEpub, uint32_t satEpubLen)
{
	//check input
	if((NULL == satEpub) || (0 == satEpubLen))
	{
		printf("_saveSatEpubToFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open file
	int fd = -1;
	fd = open(TEMPEPUBFILEPATH, O_CREAT | O_WRONLY, 0777);
	if(fd < 0)
	{
		printf("_saveSatEpubToFile():");
		printf("Failed to create and open epub.temp.\n");
		return -1;
	}

	//write satEpub into epub.temp
	int ret = -1;
	ret = write(fd, &satEpubLen, sizeof(satEpubLen));
	if(ret != sizeof(satEpubLen))
	{
		if(-1 == ret)
		{
			perror("_saveSatEpubToFile():write()");
		}
		else
		{
			printf("_saveSatEpubToFile():write():");
		}	
		printf("Failed to write length of sat Epub to epub.temp.\n");
		close(fd);
		return -1;
	}

	ret = write(fd, satEpub, satEpubLen);
	if(ret != satEpubLen)
	{
		if(-1 == ret)
		{
			perror("_saveSatEpubToFile():write()");
		}
		else
		{
			printf("_saveSatEpubToFile():write():");
		}
		printf("Failed to write satEpub to epub.temp.\n");
		close(fd);
		return -1;
	}
	
	//close file
	close(fd);
	return 0;
}


int _genTempSatEpubFile(char *uuid, uint32_t uuidLen, char *satEpub, uint32_t satEpubLen)
{
	//check 
	if((NULL == uuid) || (uuidLen != 32) || (NULL == satEpub) || (0 == satEpubLen))
	{
		printf("_genTempSatEpubFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open file
	int fd = -1;
	fd = open(TEMPSATEPUBFILEPATH, O_CREAT | O_WRONLY, 0777);
	if(fd < 0)
	{
		printf("_genTempSatEpubFile():");
		printf("Failed to open temp.satepub.\n");
		return -1;
	}

	//write uuid
	int ret = -1;
	ret = write(fd, uuid, uuidLen);
	if(ret != uuidLen)
	{
		if(-1 == ret)
		{
			perror("_genTempSatEpubFile()");
		}
		else
		{
			printf("_genTempSatEpubFile():");
		}
		printf("Failed to write uuid into temp.satepub");
		close(fd);
		return -1;
	
	}

	//write length
	ret = write(fd, &satEpubLen, sizeof(satEpubLen));
	if(ret != sizeof(satEpubLen))
	{
		if(-1 == ret)
		{
			perror("_genTempSatEpubFile()");
		}
		else
		{
			printf("_genTempSatEpubFile():");
		}
		printf("Failed to write epub length into temp.satepub");
		close(fd);
		return -1;
	}

	//write epub
	ret = write(fd, satEpub, satEpubLen);
	if(ret != satEpubLen)
	{
		if(-1 == ret)
		{
			perror("_genTempSatEpubFile()");
		}
		else
		{
			printf("_genTempSatEpubFile():");
		}
		printf("Failed to write satEpub into temp.satepub");
		close(fd);
		return -1;
	}

	//close
	close(fd);
	return 0;
}


int _genUuidSatEpubFile(char *uuid, uint32_t uuidLen)
{
	//check 
	if((NULL == uuid) || (uuidLen != 32))
	{
		printf("_genUuidSatEpubFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//get new path
	void *fileNameBuf = NULL;
	fileNameBuf = malloc(strlen(DOWNLOADDIR) + uuidLen + strlen(SATEPUBFILETAIL)+1);	
	if(NULL == fileNameBuf)
	{
		printf("_genUuidSatEpubFile():");
		printf("Failed to allocate memory for new file name.\n");
		return -1;
	}
	memset(fileNameBuf, '\0', (strlen(DOWNLOADDIR) + uuidLen + strlen(SATEPUBFILETAIL)+1));

	memcpy(fileNameBuf, DOWNLOADDIR, strlen(DOWNLOADDIR));	
	strncat(fileNameBuf, uuid, uuidLen);
	strncat(fileNameBuf, SATEPUBFILETAIL, strlen(SATEPUBFILETAIL));
	
	//rename
	int ret = -1;
	ret = rename(TEMPSATEPUBFILEPATH, fileNameBuf);
	if(-1 == ret)
	{
		perror("_genUuidSatEpubFile()");
		printf("Failed to rename temp.satepub.\n");
		free(fileNameBuf);
		return -1;

	}

	free(fileNameBuf);
	return 0;
}


//
//
int init_checkSpcEpubFile()
{
	int ret = -1;
	ret = _countSpcEpubFile(DEPLOYDIR);
	if(1 == ret)
	{
		return 0;
	}
	else if(-1 == ret)
	{
		printf("init_checkSpcEpubFile():");
		printf("Failed to perform searching ops in upload directory.\n");
		return -1;
	}
	else if(0 == ret)
	{
		printf("init_checkSpcEpubFile():");
		printf("No xxxx.spcepub file found in upload directory.\n");
		return -1;
	}
	else if(ret > 1)
	{
		printf("init_checkSpcEpubFile():");
		printf("More than one xxxx.spcepub file found in upload directory.\n");
		return -1;
	}
}


//
//
int init_exchangeEpub()
{
	//get spc epub name
	int ret = -1;
	char *spcEpubFileName = NULL;
	uint32_t spcEpubFileNameLen = 0;
	spcEpubFileName = _findSpcEpubFile(DEPLOYDIR, &spcEpubFileNameLen);	
	if((NULL == spcEpubFileName) || (0 == spcEpubFileNameLen))
	{
		printf("init_exchangeEpub():");
		printf("Failed to find uuid.spcepub in upload directory.\n");
		if(NULL != spcEpubFileName)
		{
			free(spcEpubFileName);
		}
		return -1;
	}


	//move deploy/uuid.spcepub to temp/spc.epub
	//free name buffer
	ret = _mvSpcEpubFileToTempDir(spcEpubFileName, spcEpubFileNameLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to move upload/uuid.spcepub to node/temp/spc.epub.\n");
		_freeSpcEpubFileName(spcEpubFileName);
		return -1;
	}
	_freeSpcEpubFileName(spcEpubFileName);
	

	//create epub pem file
	ret = create_epub_pem_file();
	if(-1 == ret)
	{
		printf("init_exchangeEpub():failed to gen sat eprv and epub.\n");
		return -1;
	}

	//get buffer len
	uint32_t satEpubBufferLen = 0;
	ret = get_pem_key_buffer_len(SATEPUBPEMFILEPATH, &satEpubBufferLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():failed to get pem key buffer len.\n");
		return -1;
	}

	//get epub from pem file
	uint8_t *satEpub = NULL;
	satEpub = malloc(satEpubBufferLen);
	if(NULL == satEpub)
	{
		printf("init_exchangeEpub():failed to malloc for sat epub.\n");
		return -1;
	}
	memset(satEpub, '\0', satEpubBufferLen);

	ret = pem_to_buffer(SATEPUBPEMFILEPATH, satEpub, satEpubBufferLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():failed to get sat epub from pem file.\n");
		free(satEpub);
		return -1;
	}	


	//save sat epub to epub.temp
	ret = _saveSatEpubToTempFile((char *)satEpub, satEpubBufferLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to save sat epub to epub.temp.\n");
		free(satEpub);
		return -1;
	}


	//parse spc.epub
	char uuid[UUIDLEN+1];
	memset(uuid, '\0', UUIDLEN+1);
	char *spcEpub = NULL;
	uint32_t spcEpubLen = 0;
	ret = _parseSpcEpubFile(uuid, UUIDLEN, &spcEpub, &spcEpubLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to parse spc.epub.\n");
		if(NULL != spcEpub)
		{
			free(spcEpub);
		}
		free(satEpub);
		return -1;
	}


	//save spc epub to spcpub.pem
	//free spc epub 
	ret = _saveSpcEpubToPemFile(spcEpub, spcEpubLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to save spc epub to epub.temp.\n");
		free(spcEpub);
		free(satEpub);
		return -1;
	}
	free(spcEpub);

	//gen temp.satepub
	//free sat epub	
	ret = _genTempSatEpubFile(uuid, UUIDLEN, satEpub, satEpubBufferLen);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to generate temp.satepub.\n");
		free(satEpub);
		return -1;
	}
	free(satEpub);
	
	//get uuid.satepub
	ret = _genUuidSatEpubFile(uuid, UUIDLEN);
	if(-1 == ret)
	{
		printf("init_exchangeEpub():");
		printf("Failed to move temp.satepub to uuid.satepub.\n");
		return -1;
	}

	printf("init_exchangeEpub():");
	printf("Generated download/%s.satepub, please check.\n", uuid);
	return 0;
}

//cs1
int _countUuidSpcCs1File(char *dirPath)
{
	//check inputs
	if((NULL == dirPath))
	{
		printf("_countUuidSpcCs1File():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		printf("_countUuidSpcCs1File():opendir():");
		printf("Failed to open upload directory.\n");
		return -1;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	int retCount = 0;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCCS1FILETAIL)), SPCCS1FILETAIL, strlen(SPCCS1FILETAIL)) == 0)
		{
			retCount++;
		}	
		
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		printf("_countUuidSpcCs1File():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		return -1;
	}
	 
	//return
	return retCount; 
}


char *_findUuidSpcCs1File(char *dirPath, uint32_t *retNameLen)
{
	//check inputs
	if((NULL == dirPath) || (NULL == retNameLen))
	{
		printf("_findUuidSpcCs1File():");
		printf("Got invalid inputs.\n");
		*retNameLen = 0;
		return NULL;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		perror("_findUuidSpcCs1File():opendir()");
		printf("Failed to open upload directory.\n");
		*retNameLen = 0;
		return NULL;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	void *pFileName = NULL;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCCS1FILETAIL)), SPCCS1FILETAIL, strlen(SPCCS1FILETAIL)) == 0)
		{
			pFileName = malloc(strlen(pD_st->d_name)+1);
			if(NULL == pFileName)
			{
				printf("_findUuidSpcCs1File():readdir():malloc():");
				printf("Failed to allocate memory for ret name buffer.\n");

				if(-1 == closedir(dUl))
				{
					perror("_findUuidSpcCs1File():readdir():closedir()");
					printf("Failed to excute closedir().\n");
				}
				
				*retNameLen = 0;
				return NULL;
			}

			memset(pFileName, '\0', strlen(pD_st->d_name)+1);
			memcpy(pFileName, (pD_st->d_name), strlen(pD_st->d_name));
			break;			
		}	
		
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		perror("_findUuidSpcCs1File():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		free(pFileName);
		*retNameLen = 0;
		return NULL;
	}
	 
	//return
	if(NULL == pFileName)
	{
		*retNameLen = 0;
		return NULL;
	}

	*retNameLen = strlen(pD_st->d_name);
	return pFileName; 

}


void _freeUuidSpcCs1FileName(char *pFileName)
{
	free(pFileName);
	return;
}


int _mvUuidSpcCs1ToSpcCs1(char *fileName, uint32_t fileNameLen)
{
	//check 
	if((NULL == fileName) || (0 == fileNameLen))
	{
		printf("_mvUuidSpcCs1ToSpcCs1():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//gen old path and new path
	void *pOldPath = NULL;
	pOldPath = malloc(strlen(DEPLOYDIR)+fileNameLen+1); 
	if(NULL == pOldPath)
	{	
		printf("_mvUuidSpcCs1ToSpcCs1():");
		printf("Failed to allocate memory for uuid.spccs1 file old path.\n");
		return -1;
	}
	memset(pOldPath, '\0', strlen(DEPLOYDIR)+fileNameLen+1);
	memcpy(pOldPath, DEPLOYDIR, strlen(DEPLOYDIR));
	strncat(pOldPath, fileName, fileNameLen);
	printf("OLDPATH:%s.\n", (char *)pOldPath);

	void *pNewPath = NULL;
	pNewPath = malloc(strlen(SPCCS1FILEPATH)+1); 
	if(NULL == pNewPath)
	{	
		printf("_mvUuidSpcCs1ToSpcCs1():");
		printf("Failed to allocate memory for spc.cs1 file new path.\n");
		free(pOldPath);
		return -1;
	}
	memset(pNewPath, '\0', strlen(SPCCS1FILEPATH)+1);
	memcpy(pNewPath, SPCCS1FILEPATH, strlen(SPCCS1FILEPATH));
	printf("NEWPATH:%s.\n", (char *)pNewPath);

	//move
	if(-1 == rename(pOldPath, pNewPath))
	{
		perror("_mvUuidSpcCs1ToSpcCs1()");
		printf("Failed to move uuid.spccs1 file from upload directory to temp directory.\n");
		free(pOldPath);
		free(pNewPath);
		return -1;
	}
	
	free(pOldPath);
	free(pNewPath);
	return 0;
}


int _checkSpcCs1(void)
{
	return (access(SPCCS1FILEPATH, R_OK));
}


char *_getSpcEpub(uint32_t *pSpcEpubLen)
{
	//check
	if(NULL == pSpcEpubLen)
	{
		printf("_getSpcEpub():");
		printf("Got invalid inputs.\n");
		return NULL;
	}

	int ret = -1;
	ret = access(SPCEPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return NULL;
	}	

	//get spc epub pem key buffer len 
	uint32_t length = 0;
	ret = get_pem_key_buffer_len(SPCEPUBPEMFILEPATH, &length);
	if(-1 == ret)
	{
		return NULL;
	} 

	//malloc
	void *spcEpubBuf = NULL;
	spcEpubBuf = malloc(length);
	if(NULL == spcEpubBuf)
	{
		printf("_getSpcEpub():");
		printf("Failed to allocate memory for spc epub.\n");
		*pSpcEpubLen = 0;
		return NULL;
	}	
	memset(spcEpubBuf, '\0', length);

	//pem to buffer
	ret = pem_to_buffer(SPCEPUBPEMFILEPATH, spcEpubBuf, length);
	if(-1 == ret)
	{
		printf("_getSpcEpub():");
		printf("Failed to allocate memory for spc epub.\n");
		free(spcEpubBuf);
		*pSpcEpubLen = 0;
		return NULL;
	}

	//return 
	*pSpcEpubLen = length;
	return spcEpubBuf;
}


int _saveMspcToFile(char *mSpc, uint32_t mSpcLen)
{
	//check
	if((NULL == mSpc) || (0 == mSpcLen))
	{
		printf("_saveMspcToFile():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//create and open mspc.data
	int fd = -1;
	fd = open(MSPCDATAFILEPATH, O_WRONLY | O_CREAT, 0777);
	if(fd < 0)
	{
		printf("_saveMspcToFile():open():");
		printf("Failed to create and open mspc.data.\n");
		return -1;
	}

	//write length of Mspc
	int ret = -1;
	ret = write(fd, &mSpcLen, sizeof(mSpcLen));
	if(sizeof(mSpcLen) != ret)
	{
		if(-1 == ret)
		{
			perror("_saveMspcToFile():write()");
		}
		else
		{
			printf("_saveMspcToFile():write():");
		}
		printf("Failed to write length of Mspc to mspc.data.\n");
		close(fd);
		return -1;
	}


	//write Mspc
	ret = write(fd, mSpc, mSpcLen);
	if(mSpcLen != ret)
	{
		if(-1 == ret)
		{
			perror("_saveMspcToFile():write()");
		}
		else
		{
			printf("_saveMspcToFile():write():");
		}
		printf("Failed to write Mspc to mspc.data.\n");
		close(fd);
		return -1;
	}	

	//close
	close(fd);

	return 0;
}


int _parseSpcCs1(char *uuidBuf, uint32_t uuidBufLen)
{
	//check input
	if((NULL == uuidBuf) || (uuidBufLen < 32))
	{
		printf("_parseSpcCs1():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open file
	int fd = -1;
	fd = open(SPCCS1FILEPATH, O_RDONLY);
	if(fd < 0)
	{
		perror("_parseSpcCs1():open()");
		printf("Failed to open spc.cs1 in temp dir.\n");
		return -1;	
	}
	
	
	//read uuid
	int ret = -1;
	memset(uuidBuf, '\0', UUIDLEN);
	ret = read(fd, uuidBuf, UUIDLEN);
	if(ret != UUIDLEN)
	{
		if(-1 == ret)
		{
			perror("_parseSpcCs1():malloc()");
		}
		else
		{
			printf("_parseSpcCs1():malloc():");
		}
		printf("Failed to read uuid from spc.cs1.\n");
		close(fd);
		return -1;	
			
	}
	
	close(fd);

	//return
	return 0;
}


int _genTempSatCs1(char *uuid, uint32_t uuidLen, char *mSpc, uint32_t mSpcLen)
{
	//check
	if((NULL == uuid) || (uuidLen != UUIDLEN) || (NULL == mSpc) || (0 == mSpcLen))
	{
		printf("_genTempSatCs1():");
		printf("Got invalid inputs.\n");
		return -1;
	}


	//create and open
	int fd = -1;
	fd = open(TEMPSATCS1FILEPATH, O_CREAT | O_WRONLY, 0777);
	if(fd < 0)
	{
		printf("_genTempSatCs1():open():");
		printf("Failed to create and open temp.satcs1.\n");
		return -1;

	}


	//write uuid 
	int ret = -1;
	ret = write(fd, uuid, UUIDLEN);
	if(UUIDLEN != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs1():write()");
		}
		else
		{
			printf("_genTempSatCs1():write():");
		}
		printf("Failed to write uuid into temp.satcs1.\n");
		close(fd);
		return -1;
	}


	//write length 
	uint32_t length = mSpcLen/2;
	ret = write(fd, &length, sizeof(length));
	if(sizeof(length) != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs1():write()");
		}
		else
		{
			printf("_genTempSatCs1():write():");
		}
		printf("Failed to Mspc_up length into temp.satcs1.\n");
		close(fd);
		return -1;
	}


	//write Mspc_up
	printf("_genTempSatCs1():");
	printf("MspcUpLen:%d.\n", length);

	ret = write(fd, mSpc, length);
	if(length != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs1():write()");
		}
		else
		{
			printf("_genTempSatCs1():write():");
		}
		printf("Failed to Mspc_up into temp.satcs1.\n");
		close(fd);
		return -1;
	}

	//close
	close(fd);
	return 0;
}


int _genUuidSatCs1(char *uuid, uint32_t uuidLen)
{
	//check 
	if((NULL == uuid) || (uuidLen != 32))
	{
		printf("_genUuidSatCs1():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//get new path
	void *fileNameBuf = NULL;
	fileNameBuf = malloc(strlen(DOWNLOADDIR) + uuidLen + strlen(SATCS1FILETAIL)+1);	
	if(NULL == fileNameBuf)
	{
		printf("_genUuidSatCs1():");
		printf("Failed to allocate memory for new file name.\n");
		return -1;
	}
	memset(fileNameBuf, '\0', (strlen(DOWNLOADDIR) + uuidLen + strlen(SATCS1FILETAIL)+1));

	memcpy(fileNameBuf, DOWNLOADDIR, strlen(DOWNLOADDIR));	
	strncat(fileNameBuf, uuid, uuidLen);
	strncat(fileNameBuf, SATCS1FILETAIL, strlen(SATCS1FILETAIL));
	
	//rename
	int ret = -1;
	ret = rename(TEMPSATCS1FILEPATH, fileNameBuf);
	if(-1 == ret)
	{
		perror("_genUuidSatCs1()");
		printf("Failed to rename temp.satcs1.\n");
		free(fileNameBuf);
		return -1;

	}

	free(fileNameBuf);
	return 0;
}


//
//
int init_checkSpcCs1()
{
	int ret = -1;
	ret = _countUuidSpcCs1File(DEPLOYDIR);
	if(1 == ret)
	{
		return 0;
	}
	else if(-1 == ret)
	{
		printf("init_checkSpcCs1():");
		printf("Failed to perform searching ops in upload directory.\n");
		return -1;
	}
	else if(0 == ret)
	{
		printf("init_checkSpcCs1():");
		printf("No xxxx.spccs1 file found in upload directory.\n");
		return -1;
	}
	else if(ret > 1)
	{
		printf("init_checkSpcCs1():");
		printf("More than one xxxx.spccs1 file found in upload directory.\n");
		return -1;
	}
}


int init_cs1()
{
	//find uuid.spccs1 in upload dir
	char *retFileName = NULL;
	uint32_t fileNameLen = 0;
	retFileName = _findUuidSpcCs1File(DEPLOYDIR, &fileNameLen);
	if((NULL == retFileName) || (0 == fileNameLen))
	{
		printf("init_cs1():");
		printf("No uuid.spccs1 found in upload directory.\n");
		if(NULL != retFileName)
		{
			free(retFileName);
		}
		return -1;
	}

	//move upload/uuid.spccs1 to temp/spc.cs1
	//free retFileName
	int ret = -1;
	ret = _mvUuidSpcCs1ToSpcCs1(retFileName, fileNameLen);
	if(-1 == ret)
	{
		printf("init_cs1():");
		printf("Failed to move upload/uuid.spccs1 to temp/spc.cs1.\n");
		free(retFileName);
		return -1;
	}
	_freeUuidSpcCs1FileName(retFileName);


	//check spc.cs1
	ret = _checkSpcCs1();
	if(-1 == ret)
	{
		printf("init_cs1():_checkSpcCs1():");
		printf("Failed to find spc.cs1 in temp directory.\n");
		return -1;
	}


	//check spcepub.pem
	ret = access(SPCEPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//run encrypt cmd
	uint32_t mSpcLen = 0;
	uint8_t buffer_in[7] = {'\0'};
	uint8_t *mSpc = NULL;
	memcpy(buffer_in, SECRET, 7);
	ret = spc_en_enc_and_en64(buffer_in, 7, &mSpc, &mSpcLen);
	if(-1 == ret)
	{
		return -1;
	}

	//save  Mspc to file
	ret = _saveMspcToFile(mSpc, mSpcLen);
	if(-1 == ret)
	{
		printf("init_cs1():_saveMspcToFile():");
		printf("Failed to save Mspc to file.\n");
		free(mSpc);
		return -1;
	}

	//parse spc.cs1, get uuid
	char uuid[UUIDLEN + 1];
	memset(uuid, '\0', UUIDLEN+1);
	ret = _parseSpcCs1(uuid, UUIDLEN);
	if(-1 == ret)
	{
		printf("init_cs1():_parseSpcCs1():");
		printf("Failed to parse spc.cs1.\n");
		free(mSpc);
		return -1;
	}

	//gen temp.satcs1
	//free Mspc
	ret = _genTempSatCs1(uuid, UUIDLEN, mSpc, mSpcLen);
	if(-1 == ret)
	{
		printf("init_cs1():_genTempSatCs1():");
		printf("Failed to generate temp.satcs1.\n");
		free(mSpc);
		return -1;
	}
	free(mSpc);

	//gen uuid.satcs1
	ret = _genUuidSatCs1(uuid, UUIDLEN);
	if(-1 == ret)
	{
		printf("init_cs1():_genUuidSatCs1():");
		printf("Failed to generate uuid.satcs1.\n");
		return -1;
	}

	return 0;
}
 

//
//check stage2
int _countUuidSpcCs2File(char *dirPath)
{
	//check inputs
	if((NULL == dirPath))
	{
		printf("_countUuidSpcCs2File():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		printf("_countUuidSpcCs2File():opendir():");
		printf("Failed to open upload directory.\n");
		return -1;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	int retCount = 0;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCCS2FILETAIL)), SPCCS2FILETAIL, strlen(SPCCS2FILETAIL)) == 0)
		{
			retCount++;
		}	
		
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		printf("_countUuidSpcCs2File():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		return -1;
	}
	 
	//return
	return retCount; 
}


char *_findUuidSpcCs2File(char *dirPath, uint32_t *retNameLen)
{
	//check inputs
	if((NULL == dirPath) || (NULL == retNameLen))
	{
		printf("_findUuidSpcCs2File():");
		printf("Got invalid inputs.\n");
		*retNameLen = 0;
		return NULL;
	}

	//open dir 
	DIR *dUl = NULL;
	dUl = opendir(dirPath);
	if(NULL == dUl)
	{
		perror("_findUuidSpcCs2File():opendir()");
		printf("Failed to open upload directory.\n");
		*retNameLen = 0;
		return NULL;
	}	
	
	//read dir 
	struct dirent *pD_st = NULL;
	void *pFileName = NULL;

	pD_st = readdir(dUl);
	while(NULL != pD_st)
	{
		if(strncmp(((pD_st->d_name)+strlen(pD_st->d_name)-strlen(SPCCS2FILETAIL)), SPCCS2FILETAIL, strlen(SPCCS2FILETAIL)) == 0)
		{
			pFileName = malloc(strlen(pD_st->d_name)+1);
			if(NULL == pFileName)
			{
				printf("_findUuidSpcCs1File():readdir():malloc():");
				printf("Failed to allocate memory for ret name buffer.\n");

				if(-1 == closedir(dUl))
				{
					perror("_findUuidSpcCs1File():readdir():closedir()");
					printf("Failed to excute closedir().\n");
				}
				
				*retNameLen = 0;
				return NULL;
			}

			memset(pFileName, '\0', strlen(pD_st->d_name)+1);
			memcpy(pFileName, (pD_st->d_name), strlen(pD_st->d_name));
			break;			
		}	
		
		memset(pD_st->d_name, '\0', strlen(pD_st->d_name));	
		pD_st = readdir(dUl);
	}	

	//close dir
	if(-1 == closedir(dUl))
	{
		perror("_findUuidSpcCs2File():readdir():closedir()");
		printf("Failed to excute closedir().\n");
		free(pFileName);
		*retNameLen = 0;
		return NULL;
	}
	 
	//return
	if(NULL == pFileName)
	{
		*retNameLen = 0;
		return NULL;
	}

	*retNameLen = strlen(pD_st->d_name);
	return pFileName; 

}


int _mvUuidSpcCs2ToSpcCs2(char *fileName, uint32_t fileNameLen)
{
	//check 
	if((NULL == fileName) || (0 == fileNameLen))
	{
		printf("_mvUuidSpcCs2ToSpcCs2():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//gen old path and new path
	void *pOldPath = NULL;
	pOldPath = malloc(strlen(DEPLOYDIR)+fileNameLen+1); 
	if(NULL == pOldPath)
	{	
		printf("_mvUuidSpcCs2ToSpcCs2():");
		printf("Failed to allocate memory for uuid.spccs1 file old path.\n");
		return -1;
	}
	memset(pOldPath, '\0', strlen(DEPLOYDIR)+fileNameLen+1);
	memcpy(pOldPath, DEPLOYDIR, strlen(DEPLOYDIR));
	strncat(pOldPath, fileName, fileNameLen);
	printf("OLDPATH:%s.\n", (char *)pOldPath);

	void *pNewPath = NULL;
	pNewPath = malloc(strlen(SPCCS2FILEPATH)+1); 
	if(NULL == pNewPath)
	{	
		printf("_mvUuidSpcCs2ToSpcCs2():");
		printf("Failed to allocate memory for spc.cs2 file new path.\n");
		free(pOldPath);
		return -1;
	}
	memset(pNewPath, '\0', strlen(SPCCS2FILEPATH)+1);
	memcpy(pNewPath, SPCCS2FILEPATH, strlen(SPCCS2FILEPATH));
	printf("NEWPATH:%s.\n", (char *)pNewPath);

	//move
	if(-1 == rename(pOldPath, pNewPath))
	{
		perror("_mvUuidSpcCs2ToSpcCs2()");
		printf("Failed to move uuid.spccs2 file from upload directory to temp directory.\n");
		free(pOldPath);
		free(pNewPath);
		return -1;
	}
	
	free(pOldPath);
	free(pNewPath);
	return 0;
}


void _freeUuidSpcCs2FileName(char *pFileName)
{
	free(pFileName);
	return;
}


int _checkSpcCs2()
{
	return (access(SPCCS2FILEPATH, R_OK));
}


char *_getMsat(uint32_t *pMsatLen)
{
	//check 
	if(NULL == pMsatLen)
	{
		printf("_getMsat():");
		printf("Got invalid input.\n");
		return NULL;
	}

	//open spc.cs1
	int cs1Fd = -1;
	cs1Fd = open(SPCCS1FILEPATH, O_RDONLY);
	if(cs1Fd < 0)
	{
		printf("_getMsat():");
		printf("Failed to open spc.cs1.\n");	
		*pMsatLen = 0;
		return NULL;
	}
	
	//get Msat_up length
	int ret = -1;
	uint32_t mSatUpLen = 0;
	ret = lseek(cs1Fd, UUIDLEN, SEEK_SET);
	if(UUIDLEN != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():lseek()");
			
		}
		else
		{
			printf("_getMsat():lseek():");
		}
		printf("Failed to jump to the point at the length of MsatUp.\n");
		close(cs1Fd);
		*pMsatLen = 0;
		return NULL;

	}

	ret = read(cs1Fd, &mSatUpLen, sizeof(mSatUpLen));
	if(sizeof(mSatUpLen) != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():read()");
		}			
		else
		{
			printf("_getMsat():read():");
		}
		printf("Failed to read length of Msat_up.\n");
		close(cs1Fd);
		*pMsatLen = 0;
		return NULL;
	}

	printf("mSatUpLen:%d\n", mSatUpLen);

	if((0 == mSatUpLen))
	{
		printf("_getMsat():");
		printf("Got invalid Msat_up length.\n");
		close(cs1Fd);
		*pMsatLen = 0;
		return NULL;
	}

	//open spc.cs2
	int cs2Fd = -1;
	cs2Fd = open(SPCCS2FILEPATH, O_RDONLY);
	if(cs2Fd < 0)
	{
		printf("_getMsat():");
		printf("Failed to open spc.cs2.\n");
		close(cs1Fd);	
		*pMsatLen = 0;
		return NULL;
	}
	
	


	//get length of Msat_down
	uint32_t mSatDownLen = 0;
	ret = lseek(cs2Fd, UUIDLEN, SEEK_SET);
	if(UUIDLEN != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():lseek()");
			
		}
		else
		{
			printf("_getMsat():lseek():");
		}
		printf("Failed to jump to the point at the length of MsatDown.\n");
		close(cs1Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;

	}

	ret = read(cs2Fd, &mSatDownLen, sizeof(mSatDownLen));
	if(sizeof(mSatDownLen) != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():read()");
		}			
		else
		{
			printf("_getMsat():read():");
		}
		printf("Failed to read length of Msat_down.\n");
		close(cs2Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;
	}

	printf("mSatDownLen:%d\n", mSatDownLen);
	if((0 == mSatDownLen))
	{
		printf("_getMsat():");
		printf("Got invalid Msat_down length.\n");
		close(cs1Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;
	}


	//malloc buffer	
	void *buf = NULL;
	buf = malloc(mSatUpLen + mSatDownLen);
	if(NULL == buf)
	{
		printf("_getMsat():malloc():");
		printf("Failed to malloc memory for buffer.\n");
		close(cs1Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;
	}
	memset(buf, '\0', mSatUpLen + mSatDownLen);

	//read Msat_up to buffer
	//close spc.cs1
	ret = read(cs1Fd, buf, mSatUpLen);
	if(mSatUpLen != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():read()");
		}
		else
		{
			printf("_getMsat():read():");
		}
		printf("Failed to read Msat_up.\n");
		close(cs1Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;
	}

	close(cs1Fd);

	//read Msat_down to buffer
	//close spc.cs2
	ret = read(cs2Fd, buf+mSatUpLen, mSatDownLen);
	if(mSatDownLen != ret)
	{
		if(-1 == ret)
		{
			perror("_getMsat():read()");
		}
		else
		{
			printf("_getMsat():read():");
		}
		printf("Failed to read Msat_down.\n");
		close(cs1Fd);
		close(cs2Fd);
		*pMsatLen = 0;
		return NULL;
	}

	close(cs2Fd);

	*pMsatLen = mSatUpLen + mSatDownLen;

	//return
	return buf;
}


void _freeMsat(char *pMsat)
{
	free(pMsat);
	return;
}


char *_getSatEpub(uint32_t *pSatEpubLen)
{
	//check
	if(NULL == pSatEpubLen)
	{
		printf("_getSatEpub():");
		printf("Got invalid inputs.\n");
		return NULL;
	}

	//check sat epub pem 
	int ret = -1;
	ret = access(SATEPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return NULL;
	}

	//get sat epub pem key len
	uint32_t length = 0;
	ret = get_pem_key_buffer_len(SATEPUBPEMFILEPATH, &length);
	if(-1 == ret)
	{
		return NULL;
	}	

	//malloc
	void *satEpubBuf = NULL;
	satEpubBuf = malloc(length);
	if(NULL == satEpubBuf)
	{
		printf("_getSatEpub():");
		printf("Failed to allocate memory for sat epub.\n");
		*pSatEpubLen = 0;
		return NULL;

	}	
	memset(satEpubBuf, '\0', length);
	
	ret = pem_to_buffer(SATEPUBPEMFILEPATH, satEpubBuf, length);
	if(-1 == ret)
	{
		printf("_getSatEpub():");
		printf("Failed to get sat epub.\n");
		free(satEpubBuf);
		*pSatEpubLen = 0;
		return NULL;
	}

	//return 
	*pSatEpubLen = length;
	return satEpubBuf;
}


void _freeSatEpub(char *pSatEpub)
{
	free(pSatEpub);
	return;
}


void _freeMsg(char *pMsg)
{
	free(pMsg);
	return;
}


int _checkMsg(char *msg, uint32_t msgLen)
{
	//check
	if((NULL == msg) || (0 == msgLen))
	{
		printf("_checkMsg():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//check length
	if(msgLen != strlen(THEWORDS))
	{
		return 1;
	}

	//check words
	if(0 != strncmp(msg, THEWORDS, strlen(THEWORDS)))
	{
		return 1;
	}

	return 0;
}


int _parseSpcCs2(char *uuidBuf, uint32_t uuidBufLen)
{
	//check input
	if((NULL == uuidBuf) || (uuidBufLen < 32))
	{
		printf("_parseSpcCs2():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//open file
	int fd = -1;
	fd = open(SPCCS2FILEPATH, O_RDONLY);
	if(fd < 0)
	{
		perror("_parseSpcCs2():open()");
		printf("Failed to open spc.cs2 in temp dir.\n");
		return -1;	
	}
	
	
	//read uuid
	int ret = -1;
	memset(uuidBuf, '\0', UUIDLEN);
	ret = read(fd, uuidBuf, UUIDLEN);
	if(ret != UUIDLEN)
	{
		if(-1 == ret)
		{
			perror("_parseSpcCs2():malloc()");
		}
		else
		{
			printf("_parseSpcCs2():malloc():");
		}
		printf("Failed to read uuid from spc.cs2.\n");
		close(fd);
		return -1;	
			
	}
	
	close(fd);

	//return
	return 0;
}


char *_getMspc(uint32_t *mSpcLen)
{
	//check 
	if(NULL == mSpcLen)
	{
		printf("_getMspc():");
		printf("Got invalid input.\n");
		return NULL;
	}

	//open file
	int fd = -1;
	fd = open(MSPCDATAFILEPATH, O_RDONLY);
	if(fd < 0)
	{
		printf("_getMspc():");
		printf("Failed to open mspc.data.\n");
		*mSpcLen = 0;
		return NULL;
	}

	
	//read length
	uint32_t length = 0;
	int ret = -1;
	ret = read(fd, &length, sizeof(length));
	if(sizeof(length) != ret)
	{
		if(-1 == ret)
		{
			perror("_getMspc():read()");
		}
		else
		{
			printf("_getMspc():read():");
		}
		printf("Failed to read length of Mspc.\n");
		close(fd);
		*mSpcLen = 0;
		return NULL;
	
	}

	if((length > 1024) || (0 == length))
	{
		printf("_getMspc():");
		printf("Got invalid length of Mspc.\n");
		close(fd);
		*mSpcLen = 0;
		return NULL;
	}


	//malloc buffer
	char *buf = NULL;
	buf = malloc(length);
	if(NULL == buf)
	{
		printf("_getMspc():malloc():");
		printf("Failed to malloc memory for buffer.\n");
		close(fd);
		*mSpcLen = 0;
		return NULL;
	}
	memset(buf, '\0', length);

	//read Mspc
	ret = read(fd, buf, length);
	if(length != ret)
	{
		if(-1 == ret)
		{
			perror("_getMspc():read()");
		}
		else
		{
			printf("_getMspc():read():");
		}
		printf("Failed to read Mspc from mspc.data.\n");
		close(fd);
		*mSpcLen = 0;
		return NULL;
	
	}

	//close
	close(fd);
	*mSpcLen = length;
	return buf;

}


int _genTempSatCs2(char *uuid, uint32_t uuidLen, char *mSpc, uint32_t mSpcLen)
{
	//check
	if((NULL == uuid) || (uuidLen != UUIDLEN))
	{
		printf("_genTempSatCs2():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	if(((NULL == mSpc) && (mSpcLen != 0)) || ((NULL != mSpc) && (mSpcLen == 0)))
	{
		printf("_genTempSatCs2():");
		printf("Got invalid inputs.\n");
		return -1;
	}



	//create and open
	int fd = -1;
	fd = open(TEMPSATCS2FILEPATH, O_CREAT | O_WRONLY, 0777);
	if(fd < 0)
	{
		printf("_genTempSatCs2():open():");
		printf("Failed to create and open temp.satcs2.\n");
		return -1;

	}


	//write uuid 
	int ret = -1;
	ret = write(fd, uuid, UUIDLEN);
	if(UUIDLEN != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs2():write()");
		}
		else
		{
			printf("_genTempSatCs2():write():");
		}
		printf("Failed to write uuid into temp.satcs2.\n");
		close(fd);
		return -1;
	}


	//check failed 

	//write length 
	uint32_t length = 0;
	if(0 == mSpcLen)
	{
		length = 0;
	}
	else
	{
		length = mSpcLen - (mSpcLen/2);
	}
	ret = write(fd, &length, sizeof(length));
	if(sizeof(length) != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs2():write()");
		}
		else
		{
			printf("_genTempSatCs2():write():");
		}
		printf("Failed to Mspc_down length into temp.satcs2.\n");
		close(fd);
		return -1;
	}

	if(0 == mSpcLen)
	{
		close(fd);
		return 0;
	}


	//write Mspc_up
	printf("_genTempSatCs2():");
	printf("MspcDownLen:%d.\n", length);

	ret = write(fd, mSpc+(mSpcLen/2), length);
	if(length != ret)
	{
		if(-1 == ret)
		{
			perror("_genTempSatCs2():write()");
		}
		else
		{
			printf("_genTempSatCs2():write():");
		}
		printf("Failed to Mspc_down into temp.satcs2.\n");
		close(fd);
		return -1;
	}

	//close
	close(fd);

	return 0;
}


int _genUuidSatCs2(char *uuid, uint32_t uuidLen)
{
	//check 
	if((NULL == uuid) || (uuidLen != 32))
	{
		printf("_genUuidSatCs2():");
		printf("Got invalid inputs.\n");
		return -1;
	}

	//get new path
	void *fileNameBuf = NULL;
	fileNameBuf = malloc(strlen(DOWNLOADDIR) + uuidLen + strlen(SATCS2FILETAIL)+1);	
	if(NULL == fileNameBuf)
	{
		printf("_genUuidSatCs2():");
		printf("Failed to allocate memory for new file name.\n");
		return -1;
	}
	memset(fileNameBuf, '\0', (strlen(DOWNLOADDIR) + uuidLen + strlen(SATCS2FILETAIL)+1));

	memcpy(fileNameBuf, DOWNLOADDIR, strlen(DOWNLOADDIR));	
	strncat(fileNameBuf, uuid, uuidLen);
	strncat(fileNameBuf, SATCS2FILETAIL, strlen(SATCS2FILETAIL));
	
	//rename
	int ret = -1;
	ret = rename(TEMPSATCS2FILEPATH, fileNameBuf);
	if(-1 == ret)
	{
		perror("_genUuidSatCs2()");
		printf("Failed to rename temp.satcs2.\n");
		free(fileNameBuf);
		return -1;

	}

	free(fileNameBuf);
	return 0;
}


//
//
int init_checkSpcCs2()
{
	int ret = -1;
	ret = _countUuidSpcCs2File(DEPLOYDIR);
	if(1 == ret)
	{
		return 0;
	}
	else if(-1 == ret)
	{
		printf("init_checkSpcCs2():");
		printf("Failed to perform searching ops in upload directory.\n");
		return -1;
	}
	else if(0 == ret)
	{
		printf("init_checkSpcCs2():");
		printf("No xxxx.spccs2 file found in upload directory.\n");
		return -1;
	}
	else if(ret > 1)
	{
		printf("init_checkSpcCs2():");
		printf("More than one xxxx.spccs2 file found in upload directory.\n");
		return -1;
	}
}

int init_cs2()
{
	//find xxx.spccs2
	char *retFileName = NULL;
	uint32_t retFileNameLen = 0;
	retFileName = _findUuidSpcCs2File(DEPLOYDIR, &retFileNameLen);
	if((NULL == retFileName) || (0 == retFileNameLen))
	{
		printf("init_cs2():");
		printf("No xxxx.spccs2 found in upload directory.\n");
		if(retFileName != NULL)
		{
			free(retFileName);
		}
		return -1;
	}

	//move xxx.spccs2 to temp/spc.cs2
	//free retFileName
	int ret = -1;
	ret = _mvUuidSpcCs2ToSpcCs2(retFileName, retFileNameLen);
	if(-1 == ret)
	{
		printf("init_cs2():_mvUuidSpcCs2ToSpcCs2():");
		printf("Failed to move uuid.spccs2 to temp directory.\n");
		free(retFileName);
		return -1;
	}
	_freeUuidSpcCs2FileName(retFileName);


	//check cs1 check cs2
	ret = _checkSpcCs1();
	if(-1 == ret)
	{
		printf("init_cs2():_checkSpcCs1():");
		printf("No spc.cs1 found in temp directory.\n");
		return -1;
	}

	ret = _checkSpcCs2();
	if(-1 == ret)
	{
		printf("init_cs2():_checkSpcCs2():");
		printf("No spc.cs2 found in temp directory.\n");
		return -1;
	}

	//get Msat
	char *mSat = NULL;
	uint32_t mSatLen = 0;
	mSat = _getMsat(&mSatLen);
	if((NULL == mSat) || (0 == mSatLen))
	{
		printf("init_cs2():_getMsat():");
		printf("Failed to generate Msat.\n");
		if(NULL != mSat)
		{
			free(mSat);
		}
		return -1;
	}

	//run decrypt cmd
	//free Msat
	uint8_t *mSatDe = NULL;
	uint32_t mSatDeLen = 0;
	ret = sat_de_de64_and_dec(mSat, mSatLen, &mSatDe, &mSatDeLen);
	if(-1 == ret)
	{
		free(mSat);
		return -1;
	}
	free(mSat);

	//check
	//free msg
	int check = -1;
	check = _checkMsg(mSatDe, mSatDeLen);
	if(-1 == check)
	{
		printf("init_cs2():_checkMsg():");
		printf("Failed to compare msg and the words.\n");
		free(mSatDe);
		return -1;
	}
	free(mSatDe);

	//parse cs2
	char uuid[UUIDLEN+1];
	memset(uuid, '\0', UUIDLEN+1);
	ret = _parseSpcCs2(uuid, UUIDLEN+1);
	if(-1 == ret)
	{
		printf("init_cs2():_parseSpcCs2():");
		printf("Failed to get uuid from spc.cs2.\n");
		return -1;
	}
	
	if(0 != check)
	{
		printf("init_cs2():");
		printf("Check Failed.\n");
		ret = _genTempSatCs2(uuid, UUIDLEN, NULL, 0);
	}
	else
	{
		//get mspc
		char *mSpc = NULL;
		uint32_t mSpcLen = 0;
		mSpc = _getMspc(&mSpcLen);
		if((NULL == mSpc) || (0 == mSpcLen))
		{
			printf("init_cs2():_getMspc():");
			printf("Failed to get Mspc from mspc.data.\n");
			if(mSpc != NULL)
			{
				free(mSpc);
			}
			return -1;
		}


		//gen temp.spccs2
		//free mspc
		ret = _genTempSatCs2(uuid, UUIDLEN, mSpc, mSpcLen);
		free(mSpc);
	}

	if(-1 == ret)
	{
		printf("init_cs2():_genTempSatCs2():");
		printf("Failed to generate temp.spccs2.\n");
		return -1;
	}


	//gen uuid.spccs2
	ret = _genUuidSatCs2(uuid, UUIDLEN);
	if(-1 == ret)
	{
		printf("init_cs2():_genUuidSatCs2():");
		printf("Failed to generate uuid.satcs2.\n");
		return -1;
	}
	printf("init_cs2():_genUuidSatCs2():");
	printf("Generate download/%s.satcs2.\n", uuid);

	return 0;
}

char *init_getSpcEpub(uint32_t *spcEpubLen)
{
	if(spcEpubLen == NULL)
		return NULL;
	char *ret = NULL;
	ret = _getSpcEpub(spcEpubLen);
	return ret;
}





