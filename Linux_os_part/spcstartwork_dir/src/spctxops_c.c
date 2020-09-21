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

//free yourself
int c_req_sign_tx(int sfd, uint32_t id, uint16_t index, uint8_t *tx_hex, uint64_t tx_hex_len, uint8_t **res_hex, uint64_t *res_hex_len)
{
	//
	if((sfd < 0) || (NULL == tx_hex) || (0 == tx_hex_len) || (NULL == res_hex) || (NULL == res_hex_len))
	{
		printf("input?\n");
		return -1;
	}

	//send check
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x31;	
	cmd_st.id = id;
	cmd_st.index = index;
	//
	int ret = -1;
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send check cmd.\n");
		return -1;
	} 
	
	//get check ack
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to recv check ack.\n");	
		return -1;
	}

	if((0x32 != cmd_st.cmd) || (cmd_st.id != id) || (cmd_st.index != index))
	{
		printf("wrong check ack\n");
		return -1;
	}

	//send data_send cmd
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x41;
	cmd_st.id = id;
	cmd_st.index = index;
	cmd_st.data_len = tx_hex_len;
	printf("client:data_send_cmd:data_len:%lld\n", tx_hex_len);
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send data_send cmd\n");
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	//send data
	ret = spc_send_data_to_sock(sfd, tx_hex, tx_hex_len);
	if(-1 == ret)
	{
		printf("failed to send tx_hex\n");
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	//get data_send cmd ack
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to recv data send ack\n");
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	if((cmd_st.cmd != 0x42) || (cmd_st.id != id) || (cmd_st.index != index) || (cmd_st.data_len != tx_hex_len))
	{
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	//send start cmd
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x51;
	cmd_st.id = id;
	cmd_st.index = index;
	cmd_st.data_len = tx_hex_len;
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);	
	if(-1 == ret)
	{
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	//get start cmd ack
	//get res len
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		spc_send_finish_to_sock(sfd);
		return -1;
	}	
	if((cmd_st.cmd != 0x52) || (cmd_st.id != id) || (cmd_st.index != index) || (0 == cmd_st.data_len))
	{
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	uint64_t get_res_len = 0;
	get_res_len = cmd_st.data_len;
	uint32_t get_res_len_32 = get_res_len;

	//read res data
	uint8_t *get_res_hex = NULL;
	get_res_hex = malloc(get_res_len_32);
	if(NULL == get_res_hex)
	{
		spc_send_finish_to_sock(sfd);
		return -1;
	}

	ret = spc_recv_data_from_sock(sfd, get_res_hex, get_res_len);
	if(-1 == ret)
	{
		spc_send_finish_to_sock(sfd);
		free(get_res_hex);
		return -1;
	}

	*res_hex = get_res_hex;
	*res_hex_len = get_res_len;
	
	return 0;
}

