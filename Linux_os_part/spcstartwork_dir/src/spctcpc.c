#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "spcmoscp.h"
#include "spcserverops_c.h"
#include "spctxops_c.h"
#include "spctcpc.h"
#include "spcconfig.h"
//#define SERV_ADDR	"127.0.0.1"
//#define SERV_PORT	12345
//#define LISTENQ		1024

int tcp_gen_new_keystore(uint32_t id)
{
	//socket fds
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Failed to create socket.\n");
		return -1;
	}	

	memset(&servaddr, '\0', sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);

	int ret = -1;
	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(-1 ==  ret)
	{
		return -1;
	}
	uint16_t index = 0;
	ret = c_req_gen_new_keystore(sockfd, id);
	//uint8_t xpub[113] = {'\0'};
	//ret = c_req_gen_new_keypair(sockfd, 0, &index, xpub);
	if(0 != ret)
	{
		return -1;
	}
	close(sockfd);
	return 0;
}



int tcp_gen_new_keypair(uint32_t id, uint16_t *index, uint8_t *buffer)
{
	if((NULL == index) || (NULL == buffer))
	{
		return -1;
	}

	//socket fds
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Failed to create socket.\n");
		return -1;
	}	

	memset(&servaddr, '\0', sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);

	int ret = -1;
	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(-1 ==  ret)
	{
		return -1;
	}
	uint16_t get_index = 0;
	//ret = c_req_gen_new_keystore(sockfd, id);
	uint8_t xpub[113] = {'\0'};
	ret = c_req_gen_new_keypair(sockfd, id, &get_index, xpub);
	if(0 != ret)
	{
		return -1;
	}
	close(sockfd);
	*index = get_index;
	memcpy(buffer, xpub, 113);
	return 0;
}


int tcp_sign_tx(uint32_t id, uint16_t index, uint8_t *tx_hex, uint64_t tx_hex_len, uint8_t **res_hex, uint64_t *res_hex_len)
{
	if((NULL == tx_hex) || (0 == tx_hex_len) || (NULL == res_hex) || (NULL == res_hex_len))
	{
		printf("DEBUG:INPUTS\n");
		return -1;
	}

	//socket fds
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		printf("Failed to create socket.\n");
		return -1;
	}	

	memset(&servaddr, '\0', sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, SERV_ADDR, &servaddr.sin_addr);

	int ret = -1;
	ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(-1 ==  ret)
	{
		printf("DEBUG:CONNECT FAILED\n");
		return -1;
	}
	
	ret = c_req_sign_tx(sockfd, id, index, tx_hex, tx_hex_len, res_hex, res_hex_len);
	if(0 != ret)
	{
		printf("ret:%d\n", ret);
		return -1;
	}
	close(sockfd);
	return 0;
}


