#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "spcmoscp.h"
#include "spckeystore.h"
#include "spcxkeygen.h"
#include "spcsign.h"
#include "spcdeserialize.h"
#include "spcserialize.h"


int s_do_sign_tx(int sfd, uint32_t id, uint16_t index)
{
	//	
	if((sfd < 0))
	{
		return -1;
	}	

	//get check
	//get key pair
	SPC_KEY_PAIR keypair;
	memset(&keypair, '\0', sizeof(SPC_KEY_PAIR));

	int ret = -1;	
	ret = get_key_pair(id, index, &keypair);
	if(-1 == ret)
	{
		printf("get failed\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}
	
	//return check ack
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x32;
	cmd_st.id = id;
	cmd_st.index = index;

	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send ack.\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}

	//get data_send cmd
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to recv data_send cmd.\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}
	if((cmd_st.cmd != 0x41) || (cmd_st.id != id) || (cmd_st.index != index))
	{
		printf("data_send info(id:index) does not match.\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}

	//get data_len
	uint64_t data_len = cmd_st.data_len;
	printf("data_send_cmd:data_len:%ld\n", data_len);
	if(0 == data_len)
	{
		printf("data_len error\n");	
		spc_send_error_to_sock(sfd);
		return -1;
	}

	if(data_len > 102400)
	{
		printf("data_len error\n");	
		spc_send_error_to_sock(sfd);
		return -1;
		
	}
	
	uint8_t *tx_data = NULL;
	uint32_t data_len_32 = data_len;
	tx_data = malloc(data_len_32);
	if(NULL == tx_data)
	{
		
		printf("tx_data malloc error\n");	
		spc_send_error_to_sock(sfd);
		return -1;
	}
	memset(tx_data, '\0', data_len);

	//recv data_len
	ret = spc_recv_data_from_sock(sfd, tx_data, data_len);
	if(-1 == ret)
	{
		printf("failed to recv tx data.\n");
		spc_send_error_to_sock(sfd);
		free(tx_data);
		return -1;
	}

#if 1
	uint64_t hex_i;
	for(hex_i = 0; hex_i < data_len; hex_i++)
	{
		printf("%02x", tx_data[hex_i]);
	}
	printf("\n");

#endif
	//return data_send ack 
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x42;
	cmd_st.id = id;
	cmd_st.index = index;
	cmd_st.data_len = data_len;

	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send data send ack.\n");
		spc_send_error_to_sock(sfd);
		free(tx_data);
		return -1;
	}

	//get start cmd
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	ret = spc_recv_cmd_from_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to recv start cmd.\n");
		spc_send_error_to_sock(sfd);
		free(tx_data);
		return -1;
	}
	if((cmd_st.cmd != 0x51) || (cmd_st.id != id) || (cmd_st.index != index))
	{
		printf("start cmd info(id:index) does not match.\n");
		spc_send_error_to_sock(sfd);
		free(tx_data);
		return -1;
	}
	
	//sign
	//init tx
	SPC_TX tx_st;
	memset(&tx_st, '\0', sizeof(SPC_TX));
	ret = init_tx(tx_data, data_len, &tx_st);
	if(-1 == ret)
	{
		printf("failed to init tx.\n");
		spc_send_error_to_sock(sfd);
		free(tx_data);
		return -1;
	}
	free(tx_data);

	//sign tx
	printf("SERVER:BEFORE:SIGN TX:\n");
	printf("Xpub:%s\n", keypair.xpub);
	printf("Xprv:%s\n", keypair.xprv);
	ret = sign_tx(keypair.xpub, 113, keypair.xprv, 113, &tx_st);
	if(-1 == ret)	
	{
		printf("failed to sign tx.\n");
		spc_send_error_to_sock(sfd);
		free_tx(&tx_st);
		return -1;
	}
	printf("SERVER:SIGN TX:DONE\n");

	//serialize tx
	//freetx
	uint64_t res_len = 0;
	ret = get_tx_hex_len(&tx_st, &res_len);
	if((-1 == ret) || (0 == res_len))
	{
		printf("failed to get res len.\n");
		spc_send_error_to_sock(sfd);
		free_tx(&tx_st);
		return -1;
	}

	uint8_t *res_data = NULL;
	uint32_t res_len_32 = res_len;
	res_data = malloc(res_len_32);
	if(NULL == res_data)
	{
		printf("failed to malloc for res.\n");
		spc_send_error_to_sock(sfd);
		free_tx(&tx_st);
		return -1;
	}
	memset(res_data, '\0', res_len);

	ret = serialize_tx(&tx_st, res_data, &res_len);
	if(-1 == ret)
	{
		printf("failed to serialize signed tx.\n");
		spc_send_error_to_sock(sfd);
		free(res_data);
		free_tx(&tx_st);
		return -1;
	}
	
	printf("SERVER:GOT UPDATED RES LEN:%ld", res_len);
	printf("SERVER:FREE_SIGNED_TX_ST\n");
	free_tx(&tx_st);

	//return start ack and data len
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x52;
	cmd_st.id = id;
	cmd_st.index = index;
	printf("SERVER:START_ACK:RES_LEN:%ld\n", res_len);
	cmd_st.data_len = res_len;
	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send start cmd ack.\n");
		spc_send_error_to_sock(sfd);
		free(res_data);
		return -1;
	}
	
	//return res data
	//free res
	ret = spc_send_data_to_sock(sfd, res_data, res_len);
	if(-1 == ret)
	{
		printf("failed to send res data.\n");
		spc_send_error_to_sock(sfd);
		free(res_data);
		return -1;
	}

	free(res_data);

	return 0;
}





