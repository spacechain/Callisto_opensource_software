#ifndef SPCCONFIG_H
#define SPCCONFIG_H

#define BASIC7TEST	1

#define SERV_ADDR 	"192.168.3.2"
#define SERV_PORT	12345
#define LISTENQ		1024	

#define SPCDATADIR 	"/mnt/spcdata/"

//dir
#define UPLOADDIR 	SPCDATADIR	"upload/"
#define DOWNLOADDIR 	SPCDATADIR 	"download/"
#define PREPARETEMPDIR	SPCDATADIR   	"prepare/temp/"
#define TXDIR 		SPCDATADIR 	"transaction/txfiles/"
#define DEPLOYDIR	SPCDATADIR 	"prepare/deploy/"

///mnt/spcdata/node/satpub.pem
#define SATEPUBPEMFILEPATH 	SPCDATADIR	"node/satpub.pem"
#define SATEPUBPEMFILEPATHLEN 28 

#define SATEPRVPEMFILEPATH 	SPCDATADIR	"node/satprv.pem"
#define SATEPRVPEMFILEPATHLEN 28

#define SPCEPUBPEMFILEPATH 	SPCDATADIR	"node/spcpub.pem"
#define SPCEPUBPEMFILEPATHLEN 28  

#define USEREPUBPEMFILEPATH 	SPCDATADIR	"node/userpub.pem"
#define USEREPUBPEMFILEPATHLEN 29 

#define RMSATEPUBPEMFILECMD 	"rm -f "	SATEPUBPEMFILEPATH
#define RMSATEPRVPEMFILECMD 	"rm -f "	SATEPRVPEMFILEPATH
#define RMSPCEPUBPEMFILECMD 	"rm -f "	SPCEPUBPEMFILEPATH
#define RMUSEREPUBPEMFILECMD 	"rm -f "	USEREPUBPEMFILEPATH

#define CREATESATEPRVPEMFILECMD	"openssl genrsa -out " SATEPRVPEMFILEPATH " 1024"
#define GENSATEPUBPEMFILECMD 	"openssl rsa -in " SATEPRVPEMFILEPATH " -pubout -out " SATEPUBPEMFILEPATH

#define NODETEMP	SPCDATADIR	"/node/temp/"
#define PLAINFILEPATH	NODETEMP	"plain"
#define ENCFILEPATH	NODETEMP	"enc"
#define EN64FILEPATH	NODETEMP	"en64"
#define SIGFILEPATH	NODETEMP 	"sig"
#define MD5FILEPATH	NODETEMP 	"md5"
#define VRESFILEPATH	NODETEMP	"vres"

#define SPCENCCMD	"openssl rsautl -encrypt -pubin -inkey " SPCEPUBPEMFILEPATH " -in " PLAINFILEPATH " -out " ENCFILEPATH
#define SATENCCMD	"openssl rsautl -encrypt -pubin -inkey " SATEPUBPEMFILEPATH " -in " PLAINFILEPATH " -out " ENCFILEPATH
#define USERENCCMD	"openssl rsautl -encrypt -pubin -inkey " USEREPUBPEMFILEPATH " -in " PLAINFILEPATH " -out " ENCFILEPATH
#define SATDECCMD	"openssl rsautl -decrypt -inkey " SATEPRVPEMFILEPATH " -in " ENCFILEPATH " -out " PLAINFILEPATH
#define EN64CMD		"openssl enc -base64 -e -in " ENCFILEPATH " > " EN64FILEPATH
#define DE64CMD		"openssl enc -base64 -d -in " EN64FILEPATH " > " ENCFILEPATH
#define VSIGCMD		"openssl dgst -verify " SPCEPUBPEMFILEPATH " -sha256 -signature " SIGFILEPATH " " MD5FILEPATH




#endif
