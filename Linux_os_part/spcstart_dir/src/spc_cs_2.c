#include <stdio.h>

#include "spcmodinit.h"
#include "spcmodnode.h"

#include "spc_cs_2.h"

int spc_cs_2()
{
	int ret = -1;
	ret = init_checkSpcCs2();
	if(-1 == ret)
	{
		printf("spc_cs_2():No cs2 file found.\n");
		return -1;
	}

	ret = init_cs2();
	if(-1 == ret)
	{
		printf("spc_cs_2():Check failed in stage 2.\n");
		return -1;
	}

	

	return 0;
}
