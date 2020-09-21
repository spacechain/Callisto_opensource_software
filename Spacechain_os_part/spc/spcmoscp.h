#ifndef SPCMOSCP_H
#define SPCMOSCP_H

//cmd
typedef struct
{
	uint8_t cmd;
	uint32_t id;
	uint16_t index;
	uint64_t data_len;
	uint8_t ret;

}__attribute__((packed))MOSCP_CMD; 


//data
typedef struct
{
	uint8_t type;
	//head tail
	//
	uint64_t data_len;

}__attribute__((packed))MOSCP_DATA;

int spc_recv_cmd_from_sock(int sfd, MOSCP_CMD *cmd_st);
int spc_send_cmd_to_sock(int sfd, MOSCP_CMD *cmd_st);
int spc_send_error_to_sock(int sfd);
int spc_send_finish_to_sock(int sfd);


int spc_recv_data_from_sock(int sfd, uint8_t *buffer, uint64_t buffer_len);
int spc_send_data_to_sock(int sfd, uint8_t *buffer, uint64_t buffer_len);




#endif
