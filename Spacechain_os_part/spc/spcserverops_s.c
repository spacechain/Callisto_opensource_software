#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "spcmoscp.h"
#include "spckeystore.h"
#include "spcxkeygen.h"

int s_do_gen_new_keystore(int sfd, uint32_t id)
{
	//	
	if(sfd < 0)
	{
		return -1;
	}	

	//
	int ret = -1;
	ret = gen_new_keystore(id);
	if(-1 == ret)
	{
		spc_send_error_to_sock(sfd);
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x12;
	cmd_st.id = id;

	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		return -1;
	}

	return 0;
}



int s_do_gen_new_keypair(int sfd, uint32_t id)
{
	//	
	if(sfd < 0)
	{
		return -1;
	}	

	//
	SPC_KEY_PAIR keypair;
	memset(&keypair, '\0', sizeof(SPC_KEY_PAIR));

	//
	int ret = -1;
	ret = create_single_key_pair((keypair.xpub), 113, (keypair.xprv), 113);
	if(-1 == ret)
	{	
		printf("DEBUG\n");
		spc_send_error_to_sock(sfd);
	}
	
	uint16_t index = 0;
	ret = save_key_pair(id, &keypair, &index);
	if(-1 == ret)
	{
		printf("save failed\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}

	//
	MOSCP_CMD cmd_st;
	memset(&cmd_st, '\0', sizeof(MOSCP_CMD));
	cmd_st.cmd = 0x22;
	cmd_st.id = id;
	cmd_st.index = index;
	cmd_st.data_len = 113;

	ret = spc_send_cmd_to_sock(sfd, &cmd_st);
	if(-1 == ret)
	{
		printf("failed to send ack.\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}

#if 1
	ret = spc_send_data_to_sock(sfd, keypair.xpub, 113);
	if(-1 == ret)
	{
		printf("failed to send xpub.\n");
		spc_send_error_to_sock(sfd);
		return -1;
	}
#endif

	return 0;
}





