#ifndef SPCPEM_H
#define SPCPEM_H

//need to free yourself
int get_pem_key_buffer_len(const char *pem_file_path, uint32_t *len_ptr);

int pem_to_buffer(const char *pem_file_path, uint8_t *buffer, uint32_t buffer_len);

int buffer_to_pem(const char *pem_file_path, uint8_t *buffer, uint32_t key_len);



#endif
