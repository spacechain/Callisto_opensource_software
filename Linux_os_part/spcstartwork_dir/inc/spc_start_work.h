#ifndef SPC_START_WORK_H
#define SPC_START_WORK_H
#include "spcconfig.h"

#define CLEANUPLOADDIRCMD	"rm -f " SPCDATADIR "upload/*"
#define CLEANTXFILESDIRCMD	"rm -f " SPCDATADIR "transaction/txfiles/*"

int spc_start_work(void);



#endif
