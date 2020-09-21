#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "spcconfig.h"

int spc_verify_sig_with_md5(uint8_t *sig, uint32_t sig_len, uint8_t *md5, uint32_t md5_len)
{
	//input check
	if((NULL == sig) || (0 == sig_len) || (NULL == md5) || (0 == md5_len))
	{
		return -1;
	}

	//rm sig file
	//rm md5 file
	if((access(SIGFILEPATH, F_OK)) == 0)
	{
		remove(SIGFILEPATH);
	}
	if((access(MD5FILEPATH, F_OK)) == 0)
	{
		remove(MD5FILEPATH);
	}

	int ret = -1;
	//write sig file
	int sigFd = -1;
	sigFd = open(SIGFILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(sigFd < 0)
	{
		return -1;
	}

	ret = write(sigFd, sig, sig_len);
	if(ret != sig_len)
	{
		close(sigFd);
		remove(SIGFILEPATH);
		return -1;
	}
	close(sigFd);

	//write md5 file
	int md5Fd = -1;
	md5Fd = open(MD5FILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(md5Fd < 0)
	{
		return -1;
	}

	ret = write(md5Fd, md5, md5_len);
	if(ret != md5_len)
	{
		close(md5Fd);
		remove(SIGFILEPATH);
		remove(MD5FILEPATH);
		return -1;
	}
	close(md5Fd);

	//popen
	FILE *res = NULL;
	res = popen(VSIGCMD, "r");
	if(NULL == res)
	{
		remove(SIGFILEPATH);
		remove(MD5FILEPATH);
		return -1;
	}	

	//read result
	uint8_t get_res[11] = {'\0'};
	size_t ret_size_t = 0;
	ret_size_t = fread(get_res, 1, 11, res);
	if(ret_size_t != 11)
	{
		pclose(res);
		remove(SIGFILEPATH);
		remove(MD5FILEPATH);
		return -1;
	}
	pclose(res);
	//cmp and return
	remove(SIGFILEPATH);
	remove(MD5FILEPATH);
	
	printf("SIG ");
	int i = 0;
	for(i = 0; i < 11; i ++)
	{
		printf("%c", get_res[i]);
	}
	printf("\n");

	ret = memcmp(get_res, "Verified OK", 11);
	if(0 != ret)
	{
		return -1;
	}	
	else
	{
		return 0;
	}

}












