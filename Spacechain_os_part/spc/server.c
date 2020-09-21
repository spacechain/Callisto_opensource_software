#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "spcmoscp.h"
#include "spcserverops_s.h"
#include "spctxops_s.h"

#define SERV_PORT	12345
#define LISTENQ		1024

int main(void)
{
	//socket fds
	int listenfd, connfd;
	
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1)
	{
		printf("Failed to create listen socket.\n");
		return -1;
	}	


	memset(&servaddr, '\0', sizeof(servaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
 	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);

	//gen a error struct 

	
	int ret = -1;
	MOSCP_CMD recv_cmd_st, send_cmd_st;
	memset(&recv_cmd_st, '\0', sizeof(MOSCP_CMD));
	memset(&send_cmd_st, '\0', sizeof(MOSCP_CMD));

	for( ; ; )
	{
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
		
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
		
		//ret = read cmd
		ret = spc_recv_cmd_from_sock(connfd, &recv_cmd_st);
		if(-1 == ret)
		{
			close(connfd);
			continue;
		}

		//check/create keystore/create keypair/close fd
		if(recv_cmd_st.cmd == 0x11)
		{
			printf("SERVER LOG:got cmd:0x11:gen new keystore.\n");
			
#if 1
			s_do_gen_new_keystore(connfd, recv_cmd_st.id);
#endif
		}
		else if(recv_cmd_st.cmd == 0x21)
		{
			printf("SERVER LOG:got cmd:0x21:gen new keystore.\n");
#if 1
			s_do_gen_new_keypair(connfd, recv_cmd_st.id);
#endif
		}
		else if(recv_cmd_st.cmd == 0x31)
		{
			printf("SERVER LOG:got cmd:0x31:sign transaction.\n");
			s_do_sign_tx(connfd, recv_cmd_st.id, recv_cmd_st.index);
		}	
		else
		{
			printf("SERVER ERROR:got wrong cmd:0x%02x.\n", recv_cmd_st.cmd);
			close(connfd);
			continue;
		}
		
		//if create keystore
		//ret = gen new keystore
		//get finish
		//close fd 
		//continue


		//else if create keypair
		//ret = gen new keypair
		//get finish
		//close fd
		//continue
		
		close(connfd);		


	
	}


	return 0;
}
