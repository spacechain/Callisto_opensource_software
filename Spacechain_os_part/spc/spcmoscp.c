#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "spcmoscp.h"

#define SERV_PORT	12345
#define LISTENQ		1024

//
//create keystore		0x11
//create keystore ack		0x12
//create keypair		0x21
//create keypair ack		0x22
//check 			0x31
//check ack			0x32
//data_len send			0x41
//data_len send ack		0x42
//data_len ask			0x43
//data_len ask ack		0x44
//data_len req			0x45
//start				0x51
//start ack			0x52
//finish			0xff
//error				0xee
int spc_recv_cmd_from_sock(int sfd, MOSCP_CMD *cmd_st)
{
	//check inputs
	if((NULL == cmd_st) || (sfd < 0))
	{
		return -1;
	}

	//read data from sock
	MOSCP_CMD get_cmd_st;
	memset(&get_cmd_st, '\0', sizeof(MOSCP_CMD));
	
	int last = sizeof(MOSCP_CMD);
	int counter = 0;
	int sum = 0;
	while(last > 0)
	{
		counter =  read(sfd, (uint8_t *)(&get_cmd_st) + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;

	}

	if(sum != sizeof(MOSCP_CMD))
	{
		return -1;
	}

	//copy
	memcpy(cmd_st, &get_cmd_st, sizeof(MOSCP_CMD));
	return 0;
}

//
int spc_send_cmd_to_sock(int sfd, MOSCP_CMD *cmd_st)
{
	//check input
	if((NULL == cmd_st) || (sfd < 0))
	{
		return -1;
	}

	//send
	int sum = 0;
	int last = sizeof(MOSCP_CMD);
	int counter = 0;
	while(last > 0)
	{
		counter = write(sfd, (uint8_t *)cmd_st + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;
	}

	if(sum != sizeof(MOSCP_CMD))
	{
		return -1;
	}

	return 0;
}

//
int spc_send_error_to_sock(int sfd)
{
	//check input
	if(sfd < 0)
	{
		return -1;
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0xee;


	//send
	int sum = 0;
	int last = sizeof(MOSCP_CMD);
	int counter = 0;
	while(last > 0)
	{
		counter = write(sfd, (uint8_t *)(&cmd_st) + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;
	}

	if(sum != sizeof(MOSCP_CMD))
	{
		return -1;
	}

	return 0;
}


//
int spc_send_finish_to_sock(int sfd)
{
	//check input
	if((sfd < 0))
	{
		return -1;
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0xff;

	//send
	int sum = 0;
	int last = sizeof(MOSCP_CMD);
	int counter = 0;
	while(last > 0)
	{
		counter = write(sfd, (uint8_t *)(&cmd_st) + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;
	}

	if(sum != sizeof(MOSCP_CMD))
	{
		return -1;
	}

	return 0;
}



//
int spc_recv_data_from_sock(int sfd, uint8_t *buffer, uint64_t buffer_len)
{
	if((0 > sfd) || (NULL == buffer) || (0 == buffer_len))
	{
		return -1;
	}

	int sum = 0;
	int last = buffer_len;
	int counter = 0;
	while(last > 0)
	{
		counter =  read(sfd, buffer + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;

	}

	uint64_t sum_64 = sum;

	if(sum_64 != buffer_len)
	{
		return -1;
	}

	return 0;
}


//
int spc_send_data_to_sock(int sfd, uint8_t *buffer, uint64_t buffer_len)
{
	if((0 > sfd) || (NULL == buffer) || (0 == buffer_len))
	{
		return -1;
	}

	int sum = 0;
	int last = buffer_len;
	int counter = 0;
	while(last > 0)
	{
		counter =  write(sfd, buffer + sum, last);
		if(counter < 0)
		{
			return -1;
		}
		sum += counter;
		last -= counter;

	}

	uint64_t sum_64 = sum;

	if(sum_64 != buffer_len)
	{
		return -1;
	}


	return 0;
}

