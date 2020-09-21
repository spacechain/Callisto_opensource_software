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

int c_req_gen_new_keystore(int sfd, uint32_t id)
{
	//
	if(sfd < 0)
	{
		return -1;
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x11;	
	cmd_st.id = id;
	
	//
	int ret = -1;
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		return -1;
	} 
	
	//
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		return -1;
	}


	if(0x12 == cmd_st.cmd)
	{
		if(id == cmd_st.id)
		{
			return 0;	
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

int c_req_gen_new_keypair(int sfd, uint32_t id, uint16_t *index, uint8_t *buffer)
{
	//
	if((sfd < 0) || (NULL == index) || (NULL == buffer))
	{
		return -1;
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x21;	
	cmd_st.id = id;
	
	//
	int ret = -1;
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		return -1;
	} 
	
	//
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("DEBUG:FAILED TO RECV ACK.\n");
		return -1;
	}

	int i = 0;
	for(i = 0; i < sizeof(MOSCP_CMD); i++)
	{
		printf("%02x", ((uint8_t *)(&cmd_st))[i]);
	}

	printf("\n");


	if(0x22 == cmd_st.cmd)
	{
		if((id == cmd_st.id) && (113 == cmd_st.data_len))
		{
			*index = cmd_st.index;
			printf("cmd_st:id:%d\ncmd_st:data_len:%lld\ncmd_st:index:%d\n", cmd_st.id, cmd_st.data_len, cmd_st.index);
			ret = spc_recv_data_from_sock(sfd, buffer, 113);
			if(-1 == ret)
			{
				printf("DEBUG:FAILED TO RECV XPUB\n");
				return -1;
			}
			return 0;	
		}
		else
		{
			printf("cmd_st:id:%d\ncmd_st:data_len:%lld\ncmd_st:index%d\n", cmd_st.id, cmd_st.data_len, cmd_st.index);
			return -1;
		}
	}
	else
	{
		printf("?2\n");
		return -1;
	}

	return 0;
}


