#ifndef SPCTCPC_H
#define SPCTCPC_H

//#define SERV_ADDR	"127.0.0.1"
//#define SERV_PORT	12345
//#define LISTENQ		1024	

int tcp_gen_new_keystore(uint32_t id);

int tcp_gen_new_keypair(uint32_t id, uint16_t *index, uint8_t *buffer);

int tcp_sign_tx(uint32_t id, uint16_t index, uint8_t *tx_hex, uint64_t tx_hex_len, uint8_t **res_hex, uint64_t *res_hex_len);










#endif
