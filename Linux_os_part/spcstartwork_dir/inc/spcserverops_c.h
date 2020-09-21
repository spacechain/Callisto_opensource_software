#ifndef SPCSERVEROPS_C_H
#define SPCSERVEROPS_C_H

int c_req_gen_new_keystore(int fd, uint32_t id);
int c_req_gen_new_keypair(int fd, uint32_t id, uint16_t *index, uint8_t *buffer);

#endif
