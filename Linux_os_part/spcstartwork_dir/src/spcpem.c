#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "spcconfig.h"


int get_pem_key_buffer_len(const char *pem_file_path, uint32_t *len_ptr)
{
	//check input
	if((NULL == pem_file_path) || (NULL == len_ptr))
	{
		return -1;
	}

	//check file access
	int ret = -1;
	ret = access(pem_file_path, R_OK);
	if(0 != ret)
	{
		return -1;
	}

	//open file read only
	int fd = -1;
	fd = open(pem_file_path, O_RDONLY);
	if(fd < 0)
	{
		return -1;
	}

	//lseek to the end
	uint32_t seek_len = 0;
	seek_len = lseek(fd, 0, SEEK_END);
	if(0 == seek_len)
	{
		close(fd);
		return -1;
	}
	close(fd);

	//return length
	*len_ptr = seek_len;

	return 0;
}


int pem_to_buffer(const char *pem_file_path, uint8_t *buffer, uint32_t buffer_len)
{
	//check input
	if((NULL == pem_file_path) || (NULL == buffer) || (0 == buffer_len))
	{
		return -1;
	}

	//check pem file access r ok
	int ret = -1;	
	ret = access(pem_file_path, R_OK);
	if(0 != ret)
	{
		return -1;
	}

	//open flle rd only
	int fd = -1;
	fd = open(pem_file_path, O_RDONLY);
	if(fd < 0)
	{
		return -1;
	}

	//read to buffer
	ret = read(fd, buffer, buffer_len);
	if(ret != buffer_len)
	{
		close(fd);
		return -1;
	}

	//close
	close(fd);
	return 0;
}


int buffer_to_pem(const char *pem_file_path, uint8_t *buffer, uint32_t key_len)
{
	//check input
	if((NULL == pem_file_path) || (NULL == buffer) || (0 == key_len))
	{
		return -1;
	}

	//check pem file access w ok
	int ret = -1;
#if 0 
	ret = access(pem_file_path, W_OK);
	if(0 != ret)
	{
		return -1;
	}

#endif

	//open pem file wr only
	int fd = -1;
	fd = open(pem_file_path, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd < 0)
	{
		return -1;
	}

	//write 
	ret = write(fd, buffer, key_len);
	if(ret != key_len)
	{
		close(fd);
		return -1;
	}

	//close
	close(fd);

	return 0;
}





