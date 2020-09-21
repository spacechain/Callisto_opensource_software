#ifndef SPCMODFILEOPS_H
#define SPCMODFILEOPS_H

#include "spcconfig.h"

#define DOWNLOADFILEDIR		SPCDATADIR "download/"


int fileops_getNextTxFile(const char *txPath, char *path, uint32_t len);

int fileops_genResFilePath(const char *resDirPath, const char *txFilePath, char *resFilePath, uint32_t len);

int fileops_genDownloadFilePath(const char *downloadDirPath, const char *txFilePath, char *downloadFilePath, uint32_t len);

int fileops_rmTxFile(const char *path);





#endif
