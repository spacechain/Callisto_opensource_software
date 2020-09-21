#ifndef SPCCONFIG_H
#define SPCCONFIG_H
//dir
#define UPLOADDIR "/home/twhite/20200816_loft_node_ubuntu/spcdata/upload/"
#define DOWNLOADDIR "/home/twhite/20200816_loft_node_ubuntu/spcdata/download/"
#define PREPARETEMPDIR	"/home/twhite/20200816_loft_node_ubuntu/spcdata/prepare/temp/"

#define TXDIR	"/home/twhite/20200816_loft_node_ubuntu/spcdata/transaction/txfiles/"


#define SPCKEYSTOREPATH "/home/twhite/20200527_trezor_test/trezor-firmware/crypto/tools/spckeystores/"
#define SPCKEYSTOREPATHLEN 76

#define DEPLOYDIR	"/home/twhite/20200816_loft_node_ubuntu/spcdata/prepare/deploy/"

#define SATEPUBPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/satpub.pem"

#define RMSATEPUBPEMFILECMD "rm -f /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satpub.pem"

#define SATEPUBPEMFILEPATHLEN 62 

#define SATEPRVPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/satprv.pem"
#define RMSATEPRVPEMFILECMD "rm -f /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satprv.pem"

#define SATEPRVPEMFILEPATHLEN 62 

#define SPCEPUBPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcpub.pem"
#define SPCEPRVPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcprv.pem"
#define RMSPCEPUBPEMFILECMD "rm -f /home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcpub.pem"

#define SPCEPUBPEMFILEPATHLEN 62 

#define USEREPUBPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/userpub.pem"
#define USEREPRVPEMFILEPATH "/home/twhite/20200816_loft_node_ubuntu/spcdata/node/userprv.pem"
#define RMUSEREPUBPEMFILECMD "rm -f /home/twhite/20200816_loft_node_ubuntu/spcdata/node/userpub.pem"

#define USEREPUBPEMFILEPATHLEN 63 

#define CREATESATEPRVPEMFILECMD	"openssl genrsa -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satprv.pem 1024"

#define GENSATEPUBPEMFILECMD "openssl rsa -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satprv.pem -pubout -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satpub.pem"

#define PLAINFILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain"

#define ENCFILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc"

#define EN64FILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/en64"

#define SIGFILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/sig"

#define MD5FILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/md5"

#define VRESFILEPATH	"/home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/vres"

#define SPCENCCMD	"openssl rsautl -encrypt -pubin -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcpub.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc"

#define SATENCCMD	"openssl rsautl -encrypt -pubin -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satpub.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc"

#define USERENCCMD	"openssl rsautl -encrypt -pubin -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/userpub.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc"

#define SPCDECCMD	"openssl rsautl -decrypt -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcprv.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain"

#define SATDECCMD	"openssl rsautl -decrypt -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/satprv.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain"

#define USERDECCMD	"openssl rsautl -decrypt -inkey /home/twhite/20200816_loft_node_ubuntu/spcdata/node/userprv.pem -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc -out /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/plain"

#define EN64CMD		"openssl enc -base64 -e -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc > /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/en64"

#define DE64CMD		"openssl enc -base64 -d -in /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/en64 > /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/enc"

#define VSIGCMD		"openssl dgst -verify /home/twhite/20200816_loft_node_ubuntu/spcdata/node/spcpub.pem -sha256 -signature /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/sig /home/twhite/20200816_loft_node_ubuntu/spcdata/node/temp/md5"




#endif
