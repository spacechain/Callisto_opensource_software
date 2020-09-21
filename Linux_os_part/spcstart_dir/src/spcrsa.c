#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



#include "spcconfig.h"

//need free yourself
int sat_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(SATEPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_p < 0)
	{
		return -1;
	}
	
	ret = write(fd_p, buffer_in, buffer_in_len);
	if(ret != buffer_in_len)
	{
		close(fd_p);
		return -1;
	}
	
	close(fd_p);	

	//encrypt
	system(SATENCCMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		return -1;
	}

	//base64
	system(EN64CMD);
	if((access(EN64FILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//get data from file
	uint32_t data_len = 0;
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_RDONLY);
	if(fd_64 < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_64, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	uint32_t data_len_de_format = 0;
	if((data_len % 65) == 0)
	{
		data_len_de_format = (data_len - (data_len / 65));
	}
	else
	{
		data_len_de_format = (data_len - (data_len / 65) - 1);
	}


	uint8_t *en64_ptr = NULL;
	en64_ptr = malloc(data_len_de_format);
	if(NULL == en64_ptr)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(en64_ptr, '\0', data_len_de_format);

	lseek(fd_64, 0, SEEK_SET);
	uint32_t counter = 0;
	uint8_t bug = 0;
	uint32_t times = 0;
	times = data_len / 65;
	
	if(times * 64 > data_len_de_format)
	{
		close(fd_64);
		free(en64_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	
	uint32_t i = 0;

	for(i = 0; i < times; i ++)
	{
		ret = read(fd_64, en64_ptr+counter, 64);
		if(64 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		counter += 64;
		
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
	}

	if((data_len - times*65) >= 1)
	{
		uint32_t last = data_len - times*65 - 1;
		ret = read(fd_64, en64_ptr+counter, last);
		if(last != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
		}
		counter += last;
	
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		


	}

	close(fd_64);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	if(counter != data_len_de_format)
	{
		free(en64_ptr);
		return -1;
	}
	else
	{
		//
		*buffer_out = en64_ptr;
		*buffer_out_len = data_len_de_format;

		return 0;
	}
}



//need free yourself
int sat_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(SATEPRVPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_64 < 0)
	{
		return -1;
	}

#if 0	
	uint32_t data_len_en_format = 0;
	if((buffer_in_len / 64) == 0)
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64);
	}
	else
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64) + 1;
	}
#endif

	uint32_t times = buffer_in_len / 64;
	if((times * 64) > buffer_in_len)
	{
		close(fd_64);
		return -1;
	}

	uint32_t i = 0;
	uint32_t counter = 0;
	uint8_t bug = 0x0a;
	for(i = 0; i < times; i ++)
	{
		ret = write(fd_64, buffer_in+counter, 64);
		if(ret != 64)
		{
			close(fd_64);
			return -1;
		}
		counter += 64;	
	
		ret = write(fd_64, &bug, 1);
		if(ret != 1)
		{
			close(fd_64);
			return -1;
		}
	}

	if(buffer_in_len > counter)
	{
		ret = write(fd_64, buffer_in+counter, (buffer_in_len - counter));
		if(ret != (buffer_in_len - counter))
		{
			close(fd_64);
			return -1;
		}
		counter += (buffer_in_len - counter);
		ret = write(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			return -1;
		}

	}
	
	close(fd_64);	

	//DE BASE64
	system(DE64CMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(EN64FILEPATH);
		return -1;
	}

	//DECRYPT
	system(SATDECCMD);
	if((access(PLAINFILEPATH, F_OK) != 0))
	{
		//printf("DEBUG:SAT DE ERROR\n");
		remove(EN64FILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//get data from file
	uint32_t data_len = 0;
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_RDONLY);
	if(fd_p < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_p, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	uint8_t *p_ptr = NULL;
	p_ptr = malloc(data_len);
	if(NULL == p_ptr)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(p_ptr, '\0', data_len);

	lseek(fd_p, 0, SEEK_SET);
	ret = read(fd_p, p_ptr, data_len);
	if(data_len != ret)
	{
		close(fd_p);
		free(p_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	
	}

	close(fd_p);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	//
	*buffer_out = p_ptr;
	*buffer_out_len = data_len;

	return 0;
}

//need free yourself
int spc_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(SPCEPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_p < 0)
	{
		return -1;
	}
	
	ret = write(fd_p, buffer_in, buffer_in_len);
	if(ret != buffer_in_len)
	{
		close(fd_p);
		return -1;
	}
	
	close(fd_p);	

	//encrypt
	system(SPCENCCMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		return -1;
	}

	//base64
	system(EN64CMD);
	if((access(EN64FILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//get data from file
	uint32_t data_len = 0;
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_RDONLY);
	if(fd_64 < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_64, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	uint32_t data_len_de_format = 0;
	if((data_len % 65) == 0)
	{
		data_len_de_format = (data_len - (data_len / 65));
	}
	else
	{
		data_len_de_format = (data_len - (data_len / 65) - 1);
	}


	uint8_t *en64_ptr = NULL;
	en64_ptr = malloc(data_len_de_format);
	if(NULL == en64_ptr)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(en64_ptr, '\0', data_len_de_format);

	lseek(fd_64, 0, SEEK_SET);
	uint32_t counter = 0;
	uint8_t bug = 0;
	uint32_t times = 0;
	times = data_len / 65;
	
	if(times * 64 > data_len_de_format)
	{
		close(fd_64);
		free(en64_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	
	uint32_t i = 0;

	for(i = 0; i < times; i ++)
	{
		ret = read(fd_64, en64_ptr+counter, 64);
		if(64 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		counter += 64;
		
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
	}

	if((data_len - times*65) >= 1)
	{
		uint32_t last = data_len - times*65 - 1;
		ret = read(fd_64, en64_ptr+counter, last);
		if(last != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
		}
		counter += last;
	
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		


	}

	close(fd_64);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	if(counter != data_len_de_format)
	{
		free(en64_ptr);
		return -1;
	}
	else
	{
		//
		*buffer_out = en64_ptr;
		*buffer_out_len = data_len_de_format;

		return 0;
	}
}


#if 0
//need free yourself
int spc_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(SPCEPRVPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_64 < 0)
	{
		return -1;
	}
	
	uint32_t data_len_en_format = 0;
	if((buffer_in_len / 64) == 0)
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64);
	}
	else
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64) + 1;
	}

	uint32_t times = buffer_in_len / 64;
	if((times * 64) > buffer_in_len)
	{
		close(fd_64);
		return -1;
	}

	uint32_t i = 0;
	uint32_t counter = 0;
	uint8_t bug = 0x0a;
	for(i = 0; i < times; i ++)
	{
		ret = write(fd_64, buffer_in+counter, 64);
		if(ret != 64)
		{
			close(fd_64);
			return -1;
		}
		counter += 64;	
	
		ret = write(fd_64, &bug, 1);
		if(ret != 1)
		{
			close(fd_64);
			return -1;
		}
	}

	if(buffer_in_len > counter)
	{
		ret = write(fd_64, buffer_in+counter, (buffer_in_len - counter));
		if(ret != (buffer_in_len - counter))
		{
			close(fd_64);
			return -1;
		}
		counter += (buffer_in_len - counter);
		ret = write(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			return -1;
		}

	}
	
	close(fd_64);	

	//DE BASE64
	system(DE64CMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(EN64FILEPATH);
		return -1;
	}

	//printf("DE64 DONE\n");

	//DECRYPT
	system(SPCDECCMD);
	if((access(PLAINFILEPATH, F_OK) != 0))
	{
		//printf("DEBUG:SAT DE ERROR\n");
		remove(EN64FILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//printf("SAT DE DONE\n");

	//get data from file
	uint32_t data_len = 0;
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_RDONLY);
	if(fd_p < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_p, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	char *p_ptr = NULL;
	p_ptr = malloc(data_len);
	if(NULL == p_ptr)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(p_ptr, '\0', data_len);

	lseek(fd_p, 0, SEEK_SET);
	ret = read(fd_p, p_ptr, data_len);
	if(data_len != ret)
	{
		close(fd_p);
		free(p_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	
	}

	close(fd_p);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	//
	*buffer_out = p_ptr;
	*buffer_out_len = data_len;

	return 0;
}
#endif 


//need free yourself
int user_en_enc_and_en64(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(USEREPUBPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_p < 0)
	{
		return -1;
	}
	
	ret = write(fd_p, buffer_in, buffer_in_len);
	if(ret != buffer_in_len)
	{
		close(fd_p);
		return -1;
	}
	
	close(fd_p);	

	//encrypt
	system(USERENCCMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		return -1;
	}

	//base64
	system(EN64CMD);
	if((access(EN64FILEPATH, F_OK) != 0))
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//get data from file
	uint32_t data_len = 0;
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_RDONLY);
	if(fd_64 < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_64, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	uint32_t data_len_de_format = 0;
	if((data_len % 65) == 0)
	{
		data_len_de_format = (data_len - (data_len / 65));
	}
	else
	{
		data_len_de_format = (data_len - (data_len / 65) - 1);
	}


	uint8_t *en64_ptr = NULL;
	en64_ptr = malloc(data_len_de_format);
	if(NULL == en64_ptr)
	{
		close(fd_64);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(en64_ptr, '\0', data_len_de_format);

	lseek(fd_64, 0, SEEK_SET);
	uint32_t counter = 0;
	uint8_t bug = 0;
	uint32_t times = 0;
	times = data_len / 65;
	
	if(times * 64 > data_len_de_format)
	{
		close(fd_64);
		free(en64_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	
	uint32_t i = 0;

	for(i = 0; i < times; i ++)
	{
		ret = read(fd_64, en64_ptr+counter, 64);
		if(64 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		counter += 64;
		
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
	}

	if((data_len - times*65) >= 1)
	{
		uint32_t last = data_len - times*65 - 1;
		ret = read(fd_64, en64_ptr+counter, last);
		if(last != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
		}
		counter += last;
	
		ret = read(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		if(bug != 0x0a)	
		{
			close(fd_64);
			free(en64_ptr);
			remove(PLAINFILEPATH);
			remove(ENCFILEPATH);
			remove(EN64FILEPATH);
			return -1;
	
		}
		


	}

	close(fd_64);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	if(counter != data_len_de_format)
	{
		free(en64_ptr);
		return -1;
	}
	else
	{
		//
		*buffer_out = en64_ptr;
		*buffer_out_len = data_len_de_format;

		return 0;
	}
}


#if 0
//need free yourself
int user_de_de64_and_dec(uint8_t *buffer_in, uint32_t buffer_in_len, uint8_t **buffer_out, uint32_t *buffer_out_len)
{
	//input check
	if((NULL == buffer_in) || (0 == buffer_in_len) || (NULL == buffer_out) || (NULL == buffer_out_len))
	{
		return -1;		
	}

	//check pem file access()
	int ret = -1;
	ret = access(USEREPRVPEMFILEPATH, F_OK);
	if(0 != ret)
	{
		return -1;
	}

	//check and rm tmep file
	if((access(PLAINFILEPATH, F_OK) == 0))
	{
		remove(PLAINFILEPATH);
	}
	if((access(ENCFILEPATH, F_OK) == 0))
	{
		remove(ENCFILEPATH);
	}
	if((access(EN64FILEPATH, F_OK) == 0))
	{
		remove(EN64FILEPATH);
	}

	//write buffer_in to file
	int fd_64 = -1;
	fd_64 = open(EN64FILEPATH, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd_64 < 0)
	{
		return -1;
	}
	
	uint32_t data_len_en_format = 0;
	if((buffer_in_len / 64) == 0)
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64);
	}
	else
	{
		data_len_en_format = buffer_in_len + (buffer_in_len / 64) + 1;
	}

	uint32_t times = buffer_in_len / 64;
	if((times * 64) > buffer_in_len)
	{
		close(fd_64);
		return -1;
	}

	uint32_t i = 0;
	uint32_t counter = 0;
	uint8_t bug = 0x0a;
	for(i = 0; i < times; i ++)
	{
		ret = write(fd_64, buffer_in+counter, 64);
		if(ret != 64)
		{
			close(fd_64);
			return -1;
		}
		counter += 64;	
	
		ret = write(fd_64, &bug, 1);
		if(ret != 1)
		{
			close(fd_64);
			return -1;
		}
	}

	if(buffer_in_len > counter)
	{
		ret = write(fd_64, buffer_in+counter, (buffer_in_len - counter));
		if(ret != (buffer_in_len - counter))
		{
			close(fd_64);
			return -1;
		}
		counter += (buffer_in_len - counter);
		ret = write(fd_64, &bug, 1);
		if(1 != ret)
		{
			close(fd_64);
			return -1;
		}

	}
	
	close(fd_64);	

	//DE BASE64
	system(DE64CMD);
	if((access(ENCFILEPATH, F_OK) != 0))
	{
		remove(EN64FILEPATH);
		return -1;
	}

	//printf("DE64 DONE\n");

	//DECRYPT
	system(USERDECCMD);
	if((access(PLAINFILEPATH, F_OK) != 0))
	{
		printf("DEBUG:SAT DE ERROR\n");
		remove(EN64FILEPATH);
		remove(ENCFILEPATH);
		return -1;
	}
	
	//printf("SAT DE DONE\n");

	//get data from file
	uint32_t data_len = 0;
	int fd_p = -1;
	fd_p = open(PLAINFILEPATH, O_RDONLY);
	if(fd_p < 0)
	{
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	data_len = lseek(fd_p, 0, SEEK_END);
	if(0 == data_len)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	}

	char *p_ptr = NULL;
	p_ptr = malloc(data_len);
	if(NULL == p_ptr)
	{
		close(fd_p);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
		
	}
	memset(p_ptr, '\0', data_len);

	lseek(fd_p, 0, SEEK_SET);
	ret = read(fd_p, p_ptr, data_len);
	if(data_len != ret)
	{
		close(fd_p);
		free(p_ptr);
		remove(PLAINFILEPATH);
		remove(ENCFILEPATH);
		remove(EN64FILEPATH);
		return -1;
	
	}

	close(fd_p);
	remove(PLAINFILEPATH);
	remove(ENCFILEPATH);
	remove(EN64FILEPATH);

	//
	*buffer_out = p_ptr;
	*buffer_out_len = data_len;

	return 0;
}
#endif

