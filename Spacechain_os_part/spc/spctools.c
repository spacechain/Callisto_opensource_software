#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "spctools.h"


uint8_t char_to_hex(char in_char)
{
	if((in_char >= '0') && (in_char <= '9'))
	{
		return (uint8_t)((in_char - '0'));
	}
	else if((in_char >= 'a') && (in_char <= 'f'))
	{
		return (uint8_t)((in_char - 'a' + 10));
	}
	else if((in_char >= 'A') && (in_char <= 'F'))
	{
		return (uint8_t)((in_char - 'A' + 10));
	}
	else
	{
		return 99;
	}

}


char hex_to_char(uint8_t in_hex)
{
	if((in_hex <= 9))
	{
		return (char)((in_hex + '0'));
	}
	else if((in_hex >= 10) && (in_hex <= 15))
	{
		return (char)((in_hex - 10 + 'a'));
	}
	else
	{
		return ('z');
	}


}

//int array_char_to_hex(unsigned char *ucOutData, char *cString, int iLen)
int array_char_to_hex(uint64_t in_array_len, uint8_t *in_array_char, uint64_t out_array_len, uint8_t *out_array_hex)
{
	if((in_array_char == NULL) || (out_array_hex == NULL) || (in_array_len > 2*out_array_len))
	{
		return -1;
	}	
	
	memset(out_array_hex, '\0', out_array_len);

	uint8_t *cData = in_array_char;
	uint64_t iInIndex = 0;
	uint64_t iOutIndex = 0;
	
	for(iInIndex = 0; iInIndex < in_array_len; iInIndex++)
	{
		if((iInIndex % 2) == 0)
		{
			//odd
			*(out_array_hex + iOutIndex) |= (char_to_hex(*(cData + iInIndex)) << 4);
		}else
		{
			//even
			*(out_array_hex + iOutIndex) |= (char_to_hex(*(cData + iInIndex)));
			iOutIndex++;
		}
	}	
	return (iInIndex);
}



//int spcHexToString(char *cOutString, unsigned char *ucInData, int iLen, u_int16_t bIs)
int array_hex_to_char(uint64_t in_array_len, uint8_t *in_array_hex, uint64_t out_array_len, uint8_t *out_array_char)
{
	if((in_array_hex == NULL) || (out_array_char == NULL) || (2*in_array_len > out_array_len))
	{
		return -1;
	}

	uint8_t *ucData	= in_array_hex;
	uint64_t iInIndex = 0;
	uint64_t iOutIndex = 0;
	
	for(iInIndex = 0; iInIndex < in_array_len; iInIndex++)
	{	
		*(out_array_char + iOutIndex) = hex_to_char(((*(ucData + iInIndex)) & 0xF0) >> 4);
		iOutIndex++;
		*(out_array_char + iOutIndex) = hex_to_char((*(ucData + iInIndex)) & 0x0F);
		iOutIndex++;
	}

	return (iOutIndex);
}


int array_reverse(uint64_t in_array_len, uint8_t *in_array, uint64_t out_array_len, uint8_t *out_array, uint64_t step)
{
	if((in_array == NULL) || (out_array == NULL) || (out_array_len < in_array_len))
	{
		return -1;
	}

	if((step == 2) && ((in_array_len % 2) != 0))
	{
		return -1;
	}

	uint64_t i;
	if(step == 1)
	{
		for(i = 0; i < in_array_len; i++)
		{
			out_array[in_array_len - 1 - i] = in_array[i];
		}
	}
	else if(step == 2)
	{
		for(i = 0; i < in_array_len; i += 2)
		{
			out_array[in_array_len - 1 - 1 - i] = in_array[i];
			out_array[in_array_len - 1 - i] = in_array[i+1];
		}
	}
	else
	{
		return -1;
	}





	return 0;
}

uint8_t read_varint_len(uint8_t *buffer)
{
	if(buffer == NULL)
	{
		return 0;
	}

	uint8_t head = 0;
	head = buffer[0];

	if(head < 0xfd)
	{
		return head;
	}
	else if(head == 0xfd)
	{
		return 0xfd;
	}
	else if(head == 0xfe)
	{
		return 0xfe;
	}
	else if(head == 0xff)
	{
		return 0xff;
	}
	else
	{
		return 0;
	}

}


int read_varint(uint8_t *buffer, uint8_t varint_len, uint8_t *varint_ptr)
{
	if((buffer == NULL) || (varint_ptr == NULL))
	{
		return -1;
	}

	printf("varint_len:%d\n", varint_len);

	if(varint_len == 2)
	{
		uint16_t value = 0;
		memcpy(&value, buffer, 2);
		if((value < 0xfd))
		{
			return -1;
		}
		memcpy(varint_ptr, buffer, 2);

	}
	else if(varint_len == 4)
	{
		uint32_t value = 0;
		memcpy(&value, buffer, 4);
		if((value > 0xffffffff) || (value <= 0xffff))
		{
			return -1;
		}
		memcpy(varint_ptr, buffer, 4);

	}
	else if(varint_len == 8)
	{
		uint64_t value = 0;
		memcpy(&value, buffer, 8);
		if(value <= 0xffffffff)
		{
			return -1;
		}
		memcpy(varint_ptr, buffer, 8);
	}
	else
	{
		return -1;
	}

	return 0;
}


uint8_t gen_varint_len(uint64_t num)
{
	if(num < 0xfd)	
	{
		return 1;
	}
	else if(num <= 0xffff)
	{
		return 3;
	}
	else if(num <= 0xffffffff)
	{
		return 5;
	}
	else
	{
		return 9;
	}
}


int gen_varint(uint64_t num, uint8_t varint_len, uint8_t *buffer)
{
	if(NULL == buffer)
	{
		return -1;
	}

	if(1 == varint_len)
	{
		if(num >= 0xfd)
		{
			return -1;
		}
		uint8_t varint = num;
		memcpy(buffer, &varint, sizeof(uint8_t));
		return 0;
	}
	else if(3 == varint_len)
	{
		if((num > 0xffff) || (num < 0xfd))
		{
			return -1;
		}
		uint16_t varint = num;
		buffer[0] = 0xfd;
		memcpy(buffer + sizeof(uint8_t), &varint, sizeof(uint16_t));
		return 0;
	}
	else if(5 == varint_len)
	{
		if((num > 0xffffffff) || (num <= 0xffff))
		{
			return -1;
		}
		uint32_t varint = num;
		buffer[0] = 0xfe;
		memcpy(buffer + sizeof(uint8_t), &varint, sizeof(uint32_t));
		return 0;
	}
	else if(9 == varint_len)
	{
		if(num <= 0xffffffff)
		{
			return -1;
		}
		uint64_t varint = num;
		buffer[0] = 0xff;
		memcpy(buffer + sizeof(uint8_t), &varint, sizeof(uint64_t));
		return 0;	
	}
	else
	{
		return -1;
	}

}

#if 0
//test for gen_vatint_len();
//test for gen_varint();
int main(void)
{
	uint64_t test1 = 0x12;
	uint64_t test3 = 0x1234;
	uint64_t test5 = 0x12345678;
	uint64_t test9 = 0x123456789abcdef0;

	uint8_t varint1[1] = {'\0'};
	uint8_t varint3[3] = {'\0'};
	uint8_t varint5[5] = {'\0'};
	uint8_t varint9[9] = {'\0'};
	

	int ret = -1;
	uint8_t varint_len = 0;
	int i = 0;
	printf("test1:\n");
	varint_len = gen_varint_len(test1);
	printf("varint_len:%d\n", varint_len);
	ret = gen_varint(test1, varint_len, varint1);
	if(ret != 0)
	{
		return -1;
	}
	for(i = 0; i < 1; i++)
	{
		printf("%02x", varint1[i]);
	}
	printf("\n");

	printf("test3:\n");
	varint_len = gen_varint_len(test3);
	printf("varint_len:%d\n", varint_len);
	ret = gen_varint(test3, varint_len, varint3);
	if(ret != 0)
	{
		return -1;
	}
	for(i = 0; i < 3; i++)
	{
		printf("%02x", varint3[i]);
	}
	printf("\n");

	printf("test5:\n");
	varint_len = gen_varint_len(test5);
	printf("varint_len:%d\n", varint_len);
	ret = gen_varint(test5, varint_len, varint5);
	if(ret != 0)
	{
		return -1;
	}
	for(i = 0; i < 5; i++)
	{
		printf("%02x", varint5[i]);
	}
	printf("\n");

	printf("test9:\n");
	varint_len = gen_varint_len(test9);
	printf("varint_len:%d\n", varint_len);
	ret = gen_varint(test9, varint_len, varint9);
	if(ret != 0)
	{
		return -1;
	}
	for(i = 0; i < 9; i++)
	{
		printf("%02x", varint9[i]);
	}
	printf("\n");


	



	return 0;
}






#endif



#if 0
//test for:
//read_varint_len()
//read_varint()

int main()
{
	uint8_t buffer8[9] = {0xff, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
	uint8_t buffer4[5] = {0xfe, 0x01, 0x23, 0x45, 0x67};
	uint8_t buffer2[3] = {0xfd, 0x01, 0x23};
	uint8_t buffer1[1] = {0x55};

	uint8_t varint_len = 0;
	uint8_t value1[1] = {'\0'};
	uint16_t value2 = 0;
	uint32_t value4 = 0;
	uint64_t value8 = 0;

	varint_len = read_varint_len(buffer1);
	printf("varint_len:%d\n", varint_len);

	varint_len = read_varint_len(buffer2);
	printf("varint_len:%d\n", varint_len);

	varint_len = read_varint_len(buffer4);
	printf("varint_len:%d\n", varint_len);

	varint_len = read_varint_len(buffer8);
	printf("varint_len:%d\n", varint_len);

	printf("%d\n", 0x55);
	
	int ret = 0;
	int i;
	ret = read_varint(buffer2+1, 2, (uint8_t *)(&value2));
	printf("ret:%d\n", ret);
	for(i = 0; i < 2; i ++)
	{
		printf("%02x", ((uint8_t *)(&value2))[i]);
	}
	printf("\n");
	if(value2 == 0x2301)
	{
		printf("2 checked\n");
	}
	printf("\n");


	ret = read_varint(buffer4+1, 4, (uint8_t *)(&value4));
	printf("ret:%d\n", ret);
	for(i = 0; i < 4; i ++)
	{
		printf("%02x", ((uint8_t *)(&value4))[i]);
	}
	printf("\n");
	if(value4 == 0x67452301)
	{
		printf("4 checked\n");
	}
	printf("\n");

	ret = read_varint(buffer8+1, 8, (uint8_t *)(&value8));
	printf("ret:%d\n", ret);
	for(i = 0; i < 8; i ++)
	{
		printf("%02x", ((uint8_t *)(&value8))[i]);
	}
	printf("\n");
	if(value8 == 0xefcdab8967452301)
	{
		printf("8 checked\n");
	}
	printf("\n");
	return 0;
}




#endif





#if 0
//test for:
//array hex to char
//array char to hex
//reverse array
int main()
{
	uint8_t hex_array_in[9] = {0x00, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
	uint8_t char_array_out[18] = {'\0'};
	uint8_t char_array_reverse[18] = {'\0'};

	uint8_t char_array_in[19] = {'0', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', '0', 'a'};
	uint8_t hex_array_out[10] = {'\0'};
	uint8_t hex_array_reverse[10] = {'\0'};

	int ret = -1;
	printf("array char to hex:\n");
	ret = array_char_to_hex(19, char_array_in, 10, hex_array_out);
	printf("ret: %d\n", ret);
	int i = 0;
	for(i = 0; i < 10; i++)
	{
		printf("0x%02x ", hex_array_out[i]);
	}
	printf("\n");	

	ret = array_reverse(10, hex_array_out, 10, hex_array_reverse, 1);
	printf("ret: %d\n", ret);
	for(i = 0; i < 10; i++)
	{
		printf("0x%02x ", hex_array_reverse[i]);
	}
	printf("\n");	

	printf("array hex to char:\n");
	ret = array_hex_to_char(9, hex_array_in, 18, char_array_out);
	printf("ret: %d\n", ret);
	for(i = 0; i < 18; i++)
	{
		printf("%c", char_array_out[i]);
	}
	printf("\n");

	ret = array_reverse(18, char_array_out, 18, char_array_reverse, 2);
	printf("ret: %d\n", ret);
	for(i = 0; i < 18; i++)
	{
		printf("%c", char_array_reverse[i]);
	}
	printf("\n");	

	return 0;
}

#endif



















