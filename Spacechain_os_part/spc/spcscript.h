#ifndef SPCSCRIPT_H
#define SPCSCRIPT_H

typedef struct
{
	//item_len:
	//0: command
	//1-max:data
	uint32_t item_len;
	uint8_t *item;

}__attribute__((packed))SCRIPT_ITEM; 


typedef struct
{
	uint64_t items_num;
	uint8_t *items_list; 
}__attribute__((packed))SPC_SCRIPT;


#define	OP_0 		0x00
#define OP_FALSE	0x00
#define	OP_PUSHDATA1 	0x4c
#define OP_PUSHDATA2 	0x4d
#define OP_PUSHDATA4 	0x4e
#define OP_1NEGATE 	0x4f
#define OP_RESERVED 	0x50
#define OP_1 		0x51
#define OP_TRUE 	0x51
#define OP_2 		0x52
#define OP_3 		0x53
#define OP_4 		0x54
#define OP_5 		0x55
#define OP_6 		0x56
#define OP_7 		0x57
#define OP_8 		0x58
#define OP_9 		0x59
#define OP_10 		0x5a
#define OP_11 		0x5b
#define OP_12 		0x5c
#define OP_13 		0x5d
#define OP_14 		0x5e
#define OP_15 		0x5f
#define OP_16 		0x60



int parse_script(uint8_t *buffer, uint64_t script_len, SPC_SCRIPT *script_st_ptr);

void free_script(SPC_SCRIPT *spc_script_st_ptr);


#endif
