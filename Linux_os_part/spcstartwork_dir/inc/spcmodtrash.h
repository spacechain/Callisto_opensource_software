#ifndef SPCMODTRASH_H
#define SPCMODTRASH_H
#include "spcconfig.h"

#define TRASHTEMPDIR SPCDATADIR "trash/temp/"

#define DOWNLOADDIR	SPCDATADIR "download/"

#define MVTOTRASHTEMPCMD	"mv " DOWNLOADDIR "* " TRASHTEMPDIR

#define RMTRASHTEMPCMD	"rm -f " TRASHTEMPDIR "*"

#define KEEPLISTFILEPATH	SPCDATADIR "trash/keep.list"

void _mvFileToTrashTemp();

//access
int _checkKeepList();

int _mvFileToDownloadDir();

int _cleanTrashTempDir();

//
int trash_keepFile();

#endif
