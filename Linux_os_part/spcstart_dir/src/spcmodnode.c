#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

#include "spcconfig.h"

int check_epub_pem_file(void)
{
	int ret = -1;
	ret = access(SATEPUBPEMFILEPATH, F_OK);
	if(0 == ret)
	{
		return 1;
	}
	else if(-1 == ret)
	{
		return 0;
	}
	else
	{
		return -1;
	}

}


int create_epub_pem_file(void)
{
	//create prv pem
	system(CREATESATEPRVPEMFILECMD);
	
	int ret = -1;
	ret = access(SATEPRVPEMFILEPATH, F_OK);
	if(ret != 0)
	{
		return -1;
	}

	//create pub pem
	system(GENSATEPUBPEMFILECMD);
	ret = access(SATEPUBPEMFILEPATH, F_OK);
	if(ret != 0)
	{
		return -1;
	}

	return 0;
}


int rm_pem_files(void)
{
	//rm pub pem
	system(RMSATEPUBPEMFILECMD);

	int ret = -1;
	ret = access(SATEPUBPEMFILEPATH, F_OK);
	if(0 == ret)
	{
		return -1;
	}

	//rm prv pem
	system(RMSATEPRVPEMFILECMD);
	ret = access(SATEPRVPEMFILEPATH, F_OK);
	if(0 == ret)
	{
		return -1;
	}

	//rm spc pub pem
	system(RMSPCEPUBPEMFILECMD);
	ret = access(SPCEPUBPEMFILEPATH, F_OK);
	if(0 == ret)
	{
		return -1;
	}

	
	return 0;
}








