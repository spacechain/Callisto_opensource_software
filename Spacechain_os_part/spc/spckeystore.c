#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "spcconfig.h"
#include "spckeystore.h"


//tools
int check_keystore_dir(void)
{
	int ret = -1;
	ret = access(SPCKEYSTOREPATH, F_OK);
	if(ret == -1)
	{
		return 0;
	}
	else if(ret == 0)
	{
		return 1;
	}
	else 
	{
		return -1;
	}

}

uint8_t get_num_str_len(uint32_t id)
{
	uint8_t ret = 0;
	if(id == 0)
	{
		ret = 1; 
		return ret;
	}

	uint8_t i;
	uint32_t worker = 1000000000;
	int flag = 0;

	for(i = 0; i < 10; i ++)
	{
		//printf("worker:%d\n", worker);
		if((id / worker) > 0)
		{
			flag = 1;
			break;
		}
		else
		{
			worker = worker / 10;
		}
	}
	
	if(flag == 0)
	{
		return 0;
	}

	ret = 9 - i + 1;

	return ret;
}


//static 
int gen_keystore_path(uint32_t id, uint8_t *buffer, uint32_t *path_len_ptr)
{
	if((id > 1000000000) || (NULL == buffer) || (NULL == path_len_ptr))
	{
		return -1;
	}

	uint32_t counter = 0;

	//get id str len
	uint8_t id_str_len = 0;
	id_str_len = get_num_str_len(id);
	if(0 == id_str_len)
	{
		return -1;
	}
		
	//get id str
	size_t id_str_len_size_t = id_str_len;
	//printf("id_str_len_size_t:%d.\n", id_str_len_size_t);
	uint8_t *id_str_ptr = NULL;
	id_str_ptr = malloc(id_str_len_size_t + 1);
	if(NULL == id_str_ptr) 
	{
		return -1;
	}
	memset(id_str_ptr, '\0', id_str_len_size_t + 1);
	sprintf((char *)id_str_ptr, "%d", id);

#if 0
	printf("id_str_ptr:\n");
	int debug_i = 0;
	for(debug_i = 0; debug_i < id_str_len_size_t + 1; debug_i ++)
	{
		printf("%02x", id_str_ptr[debug_i]);
	}
	printf("\n");

#endif
	
	//get keystore file path
	uint8_t *keystore_path_ptr = NULL;
	keystore_path_ptr = malloc(SPCKEYSTOREPATHLEN + id_str_len_size_t + 1);
	if(NULL == keystore_path_ptr)
	{
		free(id_str_ptr);
		return -1;
	}
	memset(keystore_path_ptr, '\0', SPCKEYSTOREPATHLEN + id_str_len_size_t + 1);
	memcpy(keystore_path_ptr, SPCKEYSTOREPATH, SPCKEYSTOREPATHLEN);
	memcpy(keystore_path_ptr + SPCKEYSTOREPATHLEN, id_str_ptr, id_str_len_size_t);
	free(id_str_ptr);	

	//printf("check_id_exist():%s\n", keystore_path_ptr);	

	memcpy(buffer, keystore_path_ptr, SPCKEYSTOREPATHLEN + id_str_len_size_t);
	free(keystore_path_ptr);
	counter = SPCKEYSTOREPATHLEN + id_str_len_size_t;

	*path_len_ptr = counter;

	return 0;
}


//
int check_keystore_exist(uint32_t id, uint8_t *buffer, uint32_t *keystore_path_len)
{
	//check input
	if(id > 1000000000)
	{
		return -1;
	}

	//gen keystore file path
	uint8_t keystore_path_buffer[SPCKEYSTOREPATHLEN + 11] = {'\0'};
	uint32_t get_keystore_path_len = 0;
	int ret = -1;
	ret = gen_keystore_path(id, keystore_path_buffer, &get_keystore_path_len);
	if(-1 == ret)
	{
		return -1;
	}

	if(get_keystore_path_len < SPCKEYSTOREPATHLEN)
	{
		return -1;
	}

	//size_t len_size_t = keystore_path_len;
	memcpy(buffer, keystore_path_buffer, get_keystore_path_len);
	*keystore_path_len = get_keystore_path_len;
	
	//check exist
	if((access((const char *)keystore_path_buffer, F_OK)) == -1)
	{
		return 0;
	}
	else if((access((const char *)keystore_path_buffer, F_OK)) == 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}

	return -1;

}


//API
int gen_new_keystore(uint32_t id)
{
	//printf("in\n");
	if(id > 1000000000)
	{
		return -1;
	}

	//printf("INPUTS CHECK DONE\n");

	//check keystore dir
	int ret = -1;
	ret = check_keystore_dir();
	if(-1 == ret)
	{
		return -1;
	}
	else if(0 == ret)
	{
		mkdir(SPCKEYSTOREPATH, 0777);
	}
		
	//printf("KEYSTORE DIR CHECK DONE\n");

	//gen keystore path
	uint8_t keystore_path[SPCKEYSTOREPATHLEN + 11] = {'\0'};
	uint32_t keystore_path_len = 0;
	
	//input check
	//check keystore exist()
	ret = check_keystore_exist(id, keystore_path, &keystore_path_len);
	//printf("%d\n", keystore_path_len);
	//printf("01%s\n", keystore_path);
	if(0 != ret)
	{
		return -1;
	}

	if(keystore_path_len < SPCKEYSTOREPATHLEN)
	{
		return -1;
	}

	//printf("EXIST DONE\n");

	//init keystore struct
	SPC_KEYSTORE keystore_st;
	memset(&keystore_st, '\0', sizeof(SPC_KEYSTORE));
	keystore_st.id = id;
	keystore_st.key_pairs_num = 0;

	//create and open keystore file
	int fd = -1;
	//printf("02%s\n", keystore_path);
	fd = open((const char *)keystore_path, O_CREAT | O_TRUNC | O_WRONLY, 0777);
	if(fd < 0)
	{
		printf("here?\n");
		return -1;
	}	
	lseek(fd, 0, SEEK_SET);	

	//printf("OPEN DONE\n");


	//write keystore struct
	ret = write(fd, &keystore_st, sizeof(SPC_KEYSTORE));
	if(ret != sizeof(SPC_KEYSTORE))
	{
		close(fd);
		return -1;
	}

	//printf("WRITE DONE\n");

	//close file
	close(fd);
	return 0;

}


//API
int save_key_pair(uint32_t id, SPC_KEY_PAIR *key_pair_st_ptr, uint16_t *index)
{
	//printf("in\n");
	if((NULL == key_pair_st_ptr) || (NULL == index))
	{
		printf("here\n");
		return -1;
	}

	//printf("INPUT CHECKDONE\n");
	//printf("in\n");
	//gen keystore path
	uint8_t keystore_path[SPCKEYSTOREPATHLEN + 11] = {'\0'};
	uint32_t keystore_path_len = 0;
	
	//input check
	//check keystore exist()
	int ret = -1;
	ret = check_keystore_exist(id, keystore_path, &keystore_path_len);
	if(-1 == ret)
	{
		printf("error\n");
		return -1;
	}

	if(0 == ret)
	{
		printf("really?\n");
		return -1;
	}

	if(keystore_path_len < SPCKEYSTOREPATHLEN)
	{
		printf("a?\n");
		return -1;
	}

	//printf("CHECK KEYSTORE EXIST DONE\n");

	//open keystore file
	int fd = -1;
	fd = open((const char *)keystore_path, O_RDWR);
	if(fd < 0)
	{
		printf("DEBUG:OPEN FILE.\n");
		return -1;
	}	
	lseek(fd, 0, SEEK_SET);	

	printf("OPEN DONE");

	//get keystore struct
	SPC_KEYSTORE keystore_st;
	memset(&keystore_st, '\0', sizeof(SPC_KEYSTORE));
	ret = read(fd, &keystore_st, sizeof(SPC_KEYSTORE));
	int len = sizeof(SPC_KEYSTORE);
	if(ret != len)
	{
		close(fd);
		printf("failed to read keystore\n");
		return -1;
	}

	printf("READ KEYSTORE DONE\n");

	if(keystore_st.id != id)	
	{
		printf("id doesn't match\n");
		close(fd);
		return -1;
	}

	printf("CHECK ID DONE\n");

	uint32_t get_index = keystore_st.key_pairs_num;
	uint16_t get_index_16 = get_index;
	*index = get_index_16;

	//lseek
	ret = lseek(fd, sizeof(SPC_KEYSTORE) + get_index*sizeof(SPC_KEY_PAIR), SEEK_SET);
	len = sizeof(SPC_KEYSTORE) + get_index*sizeof(SPC_KEY_PAIR);
	if(ret != len)
	{
		close(fd);
		return -1;
	}

	printf("FIRST LSEEK DONE\n");

	//write keypair struct
	ret = write(fd, key_pair_st_ptr, sizeof(SPC_KEY_PAIR));
	len = sizeof(SPC_KEY_PAIR);
	if(ret != len)
	{
		close(fd);
		return -1;
	}

	printf("WRITE DONE\n");

	//update keystore struct
	keystore_st.key_pairs_num += 1;
	lseek(fd, 0, SEEK_SET);
	ret = write(fd, &keystore_st, sizeof(SPC_KEYSTORE));
	len = sizeof(SPC_KEYSTORE);
	if(ret != len)
	{	
		close(fd);
		return -1;
	}	

	printf("UPDATE KEYSTORE DONE\n");

	//close file
	close(fd);
	return 0;

}


//API
int get_key_pair(uint32_t id, uint16_t index, SPC_KEY_PAIR *key_pair_st_ptr)
{
	if(NULL == key_pair_st_ptr)
	{
		return -1;
	}

	//gen keystore path
	uint8_t keystore_path[SPCKEYSTOREPATHLEN + 11] = {'\0'};
	uint32_t keystore_path_len = 0;
	
	//input check
	//check keystore exist()
	int ret = -1;
	ret = check_keystore_exist(id, keystore_path, &keystore_path_len);
	if(-1 == ret)
	{
		return -1;
	}

	if(0 == ret)
	{
		return -1;
	}

	if(keystore_path_len < SPCKEYSTOREPATHLEN)
	{
		return -1;
	}

	//open keystore file
	int fd = -1;
	fd = open((const char *)keystore_path, O_RDONLY);
	if(fd < 0)
	{
		return -1;
	}	
	lseek(fd, 0, SEEK_SET);	


	//get keystore struct
	SPC_KEYSTORE keystore_st;
	memset(&keystore_st, '\0', sizeof(SPC_KEYSTORE));
	ret = read(fd, &keystore_st, sizeof(SPC_KEYSTORE));
	int len = sizeof(SPC_KEYSTORE);
	if(ret != len)
	{
		close(fd);
		return -1;
	}

	if(keystore_st.id != id)	
	{
		close(fd);
		return -1;
	}

	if(keystore_st.key_pairs_num <= index)
	{
		close(fd);
		return -1;
	}

	//lseek
	ret = lseek(fd, sizeof(SPC_KEYSTORE) + index*sizeof(SPC_KEY_PAIR), SEEK_SET);
	len = sizeof(SPC_KEYSTORE) + index*sizeof(SPC_KEY_PAIR);
	if(ret != len)
	{
		close(fd);
		return -1;
	}

	//read key pair struct
	ret = read(fd, key_pair_st_ptr, sizeof(SPC_KEY_PAIR));
	len = sizeof(SPC_KEY_PAIR);
	if(ret != len)
	{
		close(fd);
		return -1;
	}

	//close file
	close(fd);
	return 0;

}
	

void test_print_keystore(uint32_t id)
{
	//gen keystore path
	uint8_t keystore_path[SPCKEYSTOREPATHLEN + 11] = {'\0'};
	uint32_t keystore_path_len = 0;
	
	//input check
	//check keystore exist()
	int ret = -1;
	ret = check_keystore_exist(id, keystore_path, &keystore_path_len);
	if(-1 == ret)
	{
		return;
	}

	if(0 == ret)
	{
		return;
	}

	if(keystore_path_len < SPCKEYSTOREPATHLEN)
	{
		return;
	}

	//open keystore file
	int fd = -1;
	fd = open((const char *)keystore_path, O_RDONLY);
	if(fd < 0)
	{
		return;
	}	
	lseek(fd, 0, SEEK_SET);	


	//get keystore struct
	SPC_KEYSTORE keystore_st;
	memset(&keystore_st, '\0', sizeof(SPC_KEYSTORE));
	ret = read(fd, &keystore_st, sizeof(SPC_KEYSTORE));
	if(ret != sizeof(SPC_KEYSTORE))
	{
		close(fd);
		return;
	}

	if(keystore_st.id != id)	
	{
		close(fd);
		return;
	}

	printf("keystore:id:%d:key_pairs_num:%d\n", keystore_st.id, keystore_st.key_pairs_num);

	//close file
	close(fd);
	return;

}


#if 0
int main(void)
{
	uint32_t id[11] = {1000000000, 5, 10, 120, 1230, 12340, 123450, 1234560, 12345670, 123456780, 0};
	int i = 0;
	int j = 0;
	int ret = 0;

#if 0	
	uint8_t buffer[1024] = {'\0'};		
	uint32_t path_len = 0;
	
	for(i = 0; i < 11; i++)
	{
		memset(&buffer, '\0', 1024);
		ret = gen_keystore_path(id[i], buffer, &path_len);
		for(j = 0; j < path_len; j++)
		{
			printf("%c", buffer[j]);
		}
		printf(", %d, %d\n", path_len, ret);
	}
#endif

#if 1
	//uint16_t index = 0;

	for(i = 0; i < 11; i ++)
	{
		ret = gen_new_keystore(id[i]);
		if(-1 == ret)
		{
			printf("test main():gen_new_keystore():ret:%d.\n", ret);
			return -1;
		}

	}

#if 0
	for(i = 0; i < 11; i ++)
	{
		test_print_keystore(id[i]);
	}
#endif

#endif

#if 1
	uint16_t get_index;
	SPC_KEY_PAIR key_pair_st;
	memset(&key_pair_st, '\0', sizeof(SPC_KEY_PAIR));
	memcpy(key_pair_st.xpub, "xpub", 4);
	memcpy(key_pair_st.xprv, "xprv", 4);
#endif

#if 1
for(j = 0; j < 9; j++)
{	
	for(i = 0; i < 11; i ++)
	{
		//printf("ret:%d\n", ret);
		ret = save_key_pair(id[i], &key_pair_st, &get_index);
		if(-1 == ret)
		{
			printf("test main():save_key_pair():id:%d:ret:%d\n", id[i], ret);
		}
		printf("test main():save one key pair:id:%d:index:%d.\n", id[i], get_index);
	}

	for(i = 0; i < 11; i ++)
	{
		test_print_keystore(id[i]);
	}
}
#endif 

#if 0
	ret = gen_new_keystore(1000000000);
	if(ret == -1)
	{
		printf("test main():gen failed.\n");
		return -1;
	}

	ret = save_key_pair(1000000000, &key_pair_st, &get_index);
	if(ret == -1)
	{
		printf("test main():save failed.\n");
		return -1;
	}


#endif
	memset(&key_pair_st, '\0', sizeof(SPC_KEY_PAIR));

	//get key pair
	for(i = 0; i < 11; i ++)
	{
		//printf("ret:%d\n", ret);
		ret = get_key_pair(id[i], 8, &key_pair_st);
		if(-1 == ret)
		{
			printf("test main():get_key_pair():id:%d:ret:%d\n", id[i], ret);
		}
		printf("test main():get one key pair:id:%d.\n", id[i]);
		printf("xpub:%s\n", key_pair_st.xpub);
		printf("xprv:%s\n", key_pair_st.xprv);
	}

	




	return 0;
}
#endif 
