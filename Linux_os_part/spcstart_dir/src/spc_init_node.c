#include <stdio.h>

#include "spcmodnode.h"
#include "spcmodinit.h"
#include "spc_init_node.h"

int spc_init_node(void)
{
	//rm keys
	int ret = -1;
	ret = rm_pem_files();
	if(-1 == ret)
	{
		return -1;
	}

	ret = init_checkSpcEpubFile();
	if(-1 == ret)
	{
		printf("spc_init_node():No spc epub file found.\n");
		return -1;
	}

	ret = init_exchangeEpub();
	if(-1 == ret)
	{
		printf("spc_init_node():Failed to exchange epubkey.\n");
		return -1;
	}


	return 0;
}
