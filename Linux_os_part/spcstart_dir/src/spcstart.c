#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "spc_init_node.h"
#include "spc_cs_1.h"
#include "spc_cs_2.h"


#define PATHBUFFERSIZE	1024
#define PREPAREDIRTAIL	"prepare/"
#define DEPLOYDIRTAIL	"deploy/"
#define DATAFILENAME	"spc.data"
#define RECOVERYHEADLEN	9

#define NAMEBUFFERSIZE	50
#define MD5LEN		32


int _getCmd(uint8_t *dPath)
{
	int fd = -1;
	//open
	fd = open(dPath, O_RDONLY);
	if(fd < 0)
	{
		printf("_getCmd():ERROR:failed to open spc.data, fd:%d.\n", fd);
		return -1;
	} 

	//lseek
	int ret = -1;
	ret = lseek(fd, RECOVERYHEADLEN, SEEK_SET);
	if(ret != RECOVERYHEADLEN)
	{
		printf("_getCmd():ERROR:failed to jump to cmd head offset, %d.\n", ret);
		close(fd);
		return -1;
	}


	//read head
	uint8_t head = 0;
	uint8_t cmd = 0;
	ret = read(fd, &head, sizeof(head));
	if(ret != sizeof(head))
	{
		printf("_getCmd():ERROR:failed to read cmd head, %d.\n", ret);
		close(fd);
		return -1;
	}

	if(head != 0xaa)
	{
		printf("_getCmd():ERROR:cmd head check failed, %02x.\n", head);
		close(fd);
		return -1;
	}
	
	//read cmd
	ret = read(fd, &cmd, sizeof(cmd));
	if(ret != sizeof(cmd))
	{
		printf("_getCmd():ERROR:failed to read cmd, %d\n", ret);
		close(fd);
		return -1;
	}
	printf("CMD:%02x\n", cmd);

	//close
	close(fd);

	//check cmd
	if((cmd > 0xaa) || (cmd < 0xa1))
	{
		printf("_getCmd():ERROR:got invalid cmd, %02x\n", cmd);
		return -1;
	}	

	//return 
	int cmd_int = (cmd - 0xa0);
	return cmd_int;

	//return 0;
}

int _genFile(uint8_t *dPath, uint8_t *fPath, uint32_t fPathLen)
{
	//open
	int dFd = -1;
	dFd = open(dPath, O_RDONLY);
	if(dFd < 0)
	{
		printf("_genFile():ERROR:failed to open spc.data, fd:%d.\n", dFd);
		return -1;
	}

	//lseek
	int ret = -1;
	ret = lseek(dFd, RECOVERYHEADLEN+1+1, SEEK_SET);	
	if(ret != (RECOVERYHEADLEN+1+1))
	{
		printf("_genFile():ERROR:failed to jump to file head offset, %d.\n", ret);
		close(dFd);
		return -1;
	}


	//read head and check
	uint8_t head = 0;
	ret = read(dFd, &head, sizeof(head));
	if(ret != sizeof(head))
	{
		printf("_genFile():ERROR:failed to read file head, %d.\n", ret);
		close(dFd);
		return -1;

	}
	if(head != 0xbb)
	{
		printf("_genFile():ERROR:file head check failed, %02x.\n", head);
		close(dFd);
		return -1;
	}

	//read name len
	uint32_t nameLen = 0;
	ret = read(dFd, &nameLen, sizeof(nameLen));
	if(ret != sizeof(nameLen))
	{
		printf("_genFile():ERROR:failed to get name len, %d.\n", ret);
		close(dFd);
		return -1;
	}
	if(nameLen > NAMEBUFFERSIZE)
	{
		printf("_genFile():ERROR:name too long, %d.\n", nameLen);
		close(dFd);
		return -1;
	}	

	//read name
	uint8_t fileName[NAMEBUFFERSIZE];
	memset(fileName, '\0', NAMEBUFFERSIZE);
	ret = read(dFd, fileName, nameLen);
	if(ret != nameLen)
	{
		printf("_genFile():ERROR:failed to read name from spc.data, %d.\n", ret);
		close(dFd);
		return -1;
	}

	//read md5
	uint8_t md5buffer[MD5LEN];
	memset(md5buffer, '\0', MD5LEN);
	ret = read(dFd, md5buffer, MD5LEN);
	if(ret != MD5LEN)
	{
		printf("_genFile():ERROR:failed to read MD5 from spc.data, %d.\n", ret);
		close(dFd);
		return -1;
	}

	//gen checksum file path and open
	int cFd = -1;
	strncat(fPath, "checksum", strlen("checksum"));
	printf("_genFile():LOG:checksum file path:%s.\n", fPath);

	cFd = open(fPath, O_WRONLY | O_CREAT, 0777);
	if(cFd < 0)
	{
		printf("_genFile():ERROR:failed to open checksum file, %d.\n", cFd);
		close(dFd);
		return -1;
	}

	//gen checksum file
	ret = write(cFd, md5buffer, MD5LEN);
	if(ret != MD5LEN)
	{
		printf("_genFile():ERROR:failed to write md5 checksum to checksum file, %d.\n", ret);
		close(dFd);
		close(cFd);
		return -1;
	}	

	ret = write(cFd, "  ", 2);
	if(ret != 2)
	{
		printf("_genFile():ERROR:failed to write blank char to checksum file, %d.\n", ret);
		close(dFd);
		close(cFd);
		return -1;
	}	

	//gen file path
	memset(fPath+fPathLen, '\0', (PATHBUFFERSIZE-fPathLen));
	printf("_genFile():LOG:deploy dir path after clean:%s\n", fPath);

	ret = write(cFd, fPath, fPathLen);
	if(ret != fPathLen)
	{
		printf("_genFile():ERROR:failed to write file path to checksum file, %d.\n", ret);
		close(dFd);
		close(cFd);
		return -1;
	}	
	
	ret = write(cFd, fileName, nameLen);
	if(ret != nameLen)
	{
		printf("_genFile():ERROR:failed to write file name to checksum file, %d.\n", ret);
		close(dFd);
		close(cFd);
		return -1;
	}	

	ret = write(cFd, "\n", 1);
	if(ret != 1)
	{
		printf("_genFile():ERROR:failed to write return char to checksum file, %d.\n", ret);
		close(dFd);
		close(cFd);
		return -1;
	}	

	//close checksum file
	close(cFd);


	//gen file path
	strncat(fPath, fileName, nameLen);
	printf("_genFile():LOG:file path:%s\n", fPath);

	//open
	int fFd = -1;
	fFd = open(fPath, O_WRONLY | O_CREAT, 0777);
	if(fFd < 0)
	{
		printf("_genFile:ERROR:failed to open file in deploy, %d.\n", fFd);
		close(dFd);
		close(fFd);
		return -1;
	}

	uint32_t datLen = 0;
	ret = read(dFd, &datLen, sizeof(datLen));
	if(ret != sizeof(datLen))
	{
		printf("_genFile:ERROR:failed to read data length, %d.\n", ret);
		close(dFd);
		close(fFd);
		return -1;
	}	

	//clean buffer
	memset(dPath, '\0', PATHBUFFERSIZE);

	int times = 0;
        uint32_t last = 0;
        uint32_t sum = 0;
        times = (datLen / PATHBUFFERSIZE);
        last = (datLen % PATHBUFFERSIZE);
        printf("times:%d\n", times);
        printf("last:%d\n", last);    
    
        int i;
        for(i = 0; i < times; i++)
        {
                //read
                ret = read(dFd, fPath, PATHBUFFERSIZE);
                if(ret != PATHBUFFERSIZE)
                {
                        printf("_genFile():ERROR:failed to read in spc.data, time:%d, size:%d\n", i, ret);
                        close(dFd);
                        close(fFd);
                        return -1; 
                }
    
                //write
                ret = write(fFd, fPath, PATHBUFFERSIZE);
                if(ret != PATHBUFFERSIZE)
                {
                        printf("_genFile():ERROR:failed to write to file, time:%d, size:%d\n", i, ret);
                        close(dFd);
                        close(fFd);
                        return -1; 
                     
                }
                sum += ret;
                memset(fPath, '\0', PATHBUFFERSIZE);
        }
        printf("_genFile():LOG:writing to file for general part done.\n");
	

        //read last write last
        if(last != 0)
        {
                ret = read(dFd, fPath, last);
                if(ret != last)
                {
                        printf("_genFile():ERROR:failed to read in spc.data for last part, size:%d\n", ret);
                        close(dFd);
                        close(fFd);
                        return -1;
                }

                //write
                ret = write(fFd, fPath, last);
                if(ret != last)
                {
                        printf("_genFile():ERROR:failed to write to file for last part, size:%d\n", ret);
                        close(dFd);
                        close(fFd);
                        return -1;

                }
                sum += ret;
                memset(fPath, '\0', PATHBUFFERSIZE);
        	printf("_genFile():LOG:writing to file for last part done.\n");
        }

	//close
	close(fFd);
	close(dFd);	
	
	return 0;
}

int main(int argc, char **argv)
{
	//check para
	if(argc != 2)
	{
		printf("app:spcstart:wrong para num, %d.\n", argc);
		return -1;
	}	
	
	uint8_t buffer1[PATHBUFFERSIZE];
	uint8_t buffer2[PATHBUFFERSIZE];
	memset(buffer1, '\0', PATHBUFFERSIZE);
	memset(buffer2, '\0', PATHBUFFERSIZE);

	//write workdir to buf
	memcpy(buffer1, argv[1], strlen(argv[1]));
	//gen prepare dir 
	strncat(buffer1, PREPAREDIRTAIL, strlen(PREPAREDIRTAIL));
	//write spc.data
	strncat(buffer1, DATAFILENAME, strlen(DATAFILENAME));

	//write workdir to buf
	memcpy(buffer2, argv[1], strlen(argv[1]));
	//gen prepare dir 
	strncat(buffer2, PREPAREDIRTAIL, strlen(PREPAREDIRTAIL));
	//write deploy dir
	strncat(buffer2, DEPLOYDIRTAIL, strlen(DEPLOYDIRTAIL));

	printf("spc.data file path:%s\n", buffer1);
	printf("spc.data file path len:%d\n", (int)(strlen(buffer1)));
	printf("deploy dir path:%s\n", buffer2);
	printf("deploy dir path len:%d\n", (int)(strlen(buffer2)));


	//call _getCmd(buffer1)
	int cmd = -1;
	cmd = _getCmd(buffer1);
	printf("_getCmd().ret:%d\n", cmd);

	if(cmd != 4)
	{
		//call _getfile(buffer1 buffer2 dpathlen)
		int ret = -1;
		ret = _genFile(buffer1, buffer2, strlen(buffer2));
		printf("_genFile().ret:%d\n", ret);
		if(ret == -1)
		{		
			printf("App:spcatart:ERROR:failed to generate depolyment file.\n");
		}
		else
		{
			printf("App:spcatart:LOG:depolyment file generation done.\n");
		}
	}

	//gen spcdata workdir
	memset(buffer1, '\0', PATHBUFFERSIZE);
	memcpy(buffer1, argv[1], strlen(argv[1]));
	printf("App:spcstart:LOG:spc work dir:%s\n", buffer1);
	printf("App:spcstart:LOG:spc work dir path len:%d\n", (int)(strlen(buffer1)));
	
	memset(buffer2, '\0', PATHBUFFERSIZE);

	if(cmd == 1)
	{
		printf("App:spcstart:LOG:ready to upgrade app in Linux.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sLinuxAppUpgrade.sh", strlen("script/sLinuxAppUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 2)
	{
		printf("App:spcstart:LOG:ready to upgrade script in Linux.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sLinuxScriptUpgrade.sh", strlen("script/sLinuxScriptUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 3)
	{
		printf("App:spcstart:LOG:ready to upgrade OS image for Spacechain OS.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sSpcOsUpgrade.sh", strlen("script/sSpcOsUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 4)
	{
		printf("App:spcstart:LOG:ready to recover OS image for Spacechain OS.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sSpcOsRecovery.sh", strlen("script/sSpcOsRecovery.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 5)
	{
		printf("App:spcstart:LOG:ready to upgrade app in Spacechain OS.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sSpcAppUpgrade.sh", strlen("script/sSpcAppUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 6)
	{
		printf("App:spcstart:LOG:ready to upgrade script in Spacechain OS.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sSpcScriptUpgrade.sh", strlen("script/sSpcScriptUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}		
	else if(cmd == 7)
	{
		printf("App:spcstart:LOG:ready to upgrade lib in Spacechain OS.\n");
		strncat(buffer2, buffer1, strlen(buffer1));
		strncat(buffer2, "script/sSpcLibUpgrade.sh", strlen("script/sSpcLibUpgrade.sh"));
		strncat(buffer2, " ", strlen(" "));
		strncat(buffer2, buffer1, strlen(buffer1));
		printf("App:spcstart:LOG:script calling cmd:%s\n", buffer2);
#if BASIC7TEST
		system(buffer2);
#endif
	}
	else if(cmd == 8)
	{
		printf("App:spcstart:LOG:got init cmd.\n");
		spc_init_node();
		return 0;
	}	
	else if(cmd == 9)
	{
		printf("App:spcstart:LOG:got cs1 cmd.\n");
		spc_cs_1();
		return 0;
	}	
	else if(cmd == 10)
	{
		printf("App:spcstart:LOG:got cs2 cmd.\n");
		spc_cs_2();
		return 0;
	}	


	return 0;
}
