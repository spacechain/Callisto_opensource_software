#include <stdio.h>
#include <string.h>

#include "hasher.h"

#include "spctools.h"
#include "spcdeserialize.h"
#include "spcserialize.h"
#include "spcsign.h"
#include "spckeyget.h"


int main(void)
{
	SPC_TX tx_st;

#if 0
	uint8_t tx_char[1788] = 
	"45505446ff000200000000010260263657f5afcabd8ba7a430e04495a752ad041d32b56d2af81891fe7702f3500000000000fdffffff60263657f5afcabd8ba7a430e04495a752ad041d32b56d2af81891fe7702f3500100000000fdffffff0285d60300000000002200208f252e1750dba55dd2b7cd55166ea04ff0f78ebd36bca625a69fab1e481347e160ae0a000000000022002052ce50dcb48a7027aeee5267037165a14e4eefe6c5803148b30ac7e2cc9cb17afeffffffffb82307000000000000000500473044022031f64277e6d4c6fa24aada61bb4c4cc23371e49976ae824531725b260c59f44102204e79cb816744e8290e7ae01610e49c9aca3eda826a311592014036d5646e0f810101ff01fffd0201524c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010000004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80010000004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0100000053aefeffffffff20a10700000000000000050001ff47304402202e162149363ec7e16f47bf08cbffbc19b6ad3e0102272419fbcf582a83e647b502204274cb878dbb1f973c3addc9ad492461913b37b95cc1f19b91a613f9caa403c20101fffd0201524c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb80000002004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189000002004c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d0000020053aef5c10900";

	uint8_t tx_hex[1788] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(1788, tx_char, 1788, tx_hex);

	ret = init_tx(tx_hex, 894, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	

#endif 

//tx 00
#if 0
	uint8_t tx_char[1000] = 
	"45505446ff00020000000001019aa6460410a635cccb691ecd4c6d90d487cc7f0cf6a58b50bfa8543da984b3b30000000000fdffffff0250c30000000000002200202e285b313f967fe144ebfbcc1f17d8b734baa48dbdf31bad4ba28367dc0b0cccd3bf020000000000220020d85260a4edbc672c90ef06a22c3e2dca3c60a5099eaf71d307888c17f19cef25feffffffff85d60300000000000000050001ff483045022100cec0cbb5c6bf77d62ae715300fa70baa7bdeec465bec92c46e9c0f5c5d7f03f902203a2c88733919134011bbe3368de9b28726c0ebab1847abb43daa97818ebc5d360101fffd0201524c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d010001004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010001004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb800100010053aea9d10900";
	uint8_t tx_hex[501] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(1000, tx_char, 501, tx_hex);

	ret = init_tx(tx_hex, 500, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif

//tx 01
#if 0
	uint8_t tx_char[980] = 
	"45505446ff00020000000001019aa6460410a635cccb691ecd4c6d90d487cc7f0cf6a58b50bfa8543da984b3b30000000000fdffffff0250c30000000000001976a914fb5322642cfd5c2de49efc129cf2950d0eaa605488ac8fc3020000000000220020d85260a4edbc672c90ef06a22c3e2dca3c60a5099eaf71d307888c17f19cef25feffffffff85d60300000000000000050001ff473044022045cafb9c8e434b021ed64653b70ed850f8d1b97012084dce979431b23fb562f00220250505fd26debd28ef71fa834ef2cde2f311b3e6a3daa8b1fb72714a4d65ea160101fffd0201524c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d010001004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010001004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb800100010053aea9d10900";
	uint8_t tx_hex[491] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(980, tx_char, 491, tx_hex);

	ret = init_tx(tx_hex, 490, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif

//tx 02
#if 0
	uint8_t tx_char[1000] = 
	"45505446ff00020000000001019aa6460410a635cccb691ecd4c6d90d487cc7f0cf6a58b50bfa8543da984b3b30000000000fdffffff0250c30000000000002200202e285b313f967fe144ebfbcc1f17d8b734baa48dbdf31bad4ba28367dc0b0cccd3bf020000000000220020d85260a4edbc672c90ef06a22c3e2dca3c60a5099eaf71d307888c17f19cef25feffffffff85d603000000000000000500483045022100b00bbba3446320f9e618d77457475222e71357989c31be5d116d0e116054d08a022014ba9cabb1cc513f3a00d1eda55c0eb17443742467c75a57483b6bf9b6706e670101ff01fffd0201524c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d010001004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010001004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb800100010053aea9d10900";
	uint8_t tx_hex[501] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(1000, tx_char, 501, tx_hex);

	ret = init_tx(tx_hex, 500, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif

//tx 03
#if 0
	uint8_t tx_char[980] = 
	"45505446ff00020000000001019aa6460410a635cccb691ecd4c6d90d487cc7f0cf6a58b50bfa8543da984b3b30000000000fdffffff0250c30000000000001976a914fb5322642cfd5c2de49efc129cf2950d0eaa605488ac8fc3020000000000220020d85260a4edbc672c90ef06a22c3e2dca3c60a5099eaf71d307888c17f19cef25feffffffff85d603000000000000000500473044022016837db4d288e6280fb1b2aa809cf099e5d31595b0dabac9d7867e199b75dd9e02206ab46e67e0efae3b0d8b921345d7f8ec913b3c47b7c12b3a3561169559523aba0101ff01fffd0201524c53ff02aa7ed301d757d45d80000001fbd7cf63079ed40f95b9f888423f4c1500b5d31b31d4796a4bfd83bd0d11b7cb03fa748212fe3de3952f8d14509d95cf7baa885601389a030958d11fb81fa12b7d010001004c53ff02aa7ed301576f67d680000001d26e136d8aa889d02babf232ea3a91549b5225e6124a459bba209dcda40e0b25025977efa387180c2e024f6b2e78f6624b25cba7f903d82b3f268b2b8ff9bf8189010001004c53ff02aa7ed3013fb041b480000001997d3989f73cfc78ae882338e25e1484ce5d75baf518b01e13bc37c1fe95377603181a8f55344b2ee21e623a9ef8a983a2da317a8bbdc955feac739b99fd62eb800100010053aea9d10900";
	uint8_t tx_hex[491] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(980, tx_char, 491, tx_hex);

	ret = init_tx(tx_hex, 490, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif

//tx 04
#if 0
	uint8_t tx_char[912] = 
	"45505446ff0002000000000101879799486108dded9ca7f4dc33b2591d549cb6e864d2fe9e9e90635a7cee109c0000000000fdffffff01eeed0d00000000002200202e285b313f967fe144ebfbcc1f17d8b734baa48dbdf31bad4ba28367dc0b0cccfeffffffff96260e00000000000000050001ff01ff4730440220184487de22fa6eea4b618110162788328ac867a3f7504a0e0604d39dc1a96e2b0220517dd9632ff547cdc5b6fb9e014233190f740cb255da769eb7adca4ff3c453e401fd0201524c53ff02aa7ed301033722f4800000013ed6fa8c481793e1ec24de17b420d250fe170bf009100d09c1d44a15e2172b5b0357fe1373faddaf98e290855522d4e7a9b6e3a82a881c2ecca24f1deff50561d6000000004c53ff02aa7ed301e90c4b0a800000014369a7d0d4e102232e228d226bce458eeb8a025f2be49c9796e45097ee9c10fe025e8678be3c97f8fe04131e69f18281a1c4ed80d17ec9119375ff5ea8e245764b000000004c53ff02aa7ed301352c2fd98000000126b2447d6cc8448ff4f9d92d7ac22fd7a0bdaa34caad0d8b10a67cad24c71847027270b3d50938e8be837835ce4b33a8c95bd3bb0db4baa312090fa7c4723c0b100000000053ae3cd20900";
	uint8_t tx_hex[456] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(912, tx_char, 456, tx_hex);

	ret = init_tx(tx_hex, 456, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif

//tx 05
#if 1
	uint8_t tx_char[1748] = 
	"45505446ff00020000000001026b98eff2eed8b3d2465f270ffd5b44e627e534352dcd77ea1537f4b33e12b6e40100000000fdfffffff9c23ef6d25841b5e0c9ba3ad48de53f752812af35a45182d2c95c8fee072dfa0100000000fdffffff020c420100000000001976a914e2f737b9524d52c9ef7f2ab31999ea6c277650cc88aca0860100000000001600141111460916f2d79c0bcb263f306674150726829ffeffffffffa0860100000000000000050047304402200453a66c3fa4c996357ac900a88b556f1fe4bd43986958fe6c2e52840bd80b7702202243658dac933e1766159678878aa2e49fb88cd3c4a2177b1483334d970b54e60101ff01fffd0201524c53ff02aa7ed3012618a41980000000b97b915b2d01007e57641495b7af8ce9d0ddca3c57221cfbef97980eee639e2202b8771528e198c7f459ea1912b475cf52bf0b6439b715ce1df2daf17659721dc3000001004c53ff02aa7ed3012618a41980000001a1651d594c08a8b574e414dbaa7b057d372cb08c70e34d8aabc4f5abc724a88202d83f14c4ce9b1fab35a842a3d6678ebf4953100925760dd870796ddc45fd5963000001004c53ff02aa7ed3011fdf667780000001eeb20ff31f20c04c0029b38028a3b4b4ee0aa13ffac27b656e92d6515b7151bc03af1f54f20758e692dd046534666df8e83463f894d08a20c1d65aea36a8f400580000010053aefeffffffffa0860100000000000000050001ff01ff483045022100e4bb428edf476429699bf5a6505e6790e0219d5de208030048ddedc1234e9a7b022015588dae0cd5ea1c6606da444de2c1df347d6f3a8796edf4e43a62b319d4bfe501fd0201524c53ff02aa7ed3012618a41980000001a1651d594c08a8b574e414dbaa7b057d372cb08c70e34d8aabc4f5abc724a88202d83f14c4ce9b1fab35a842a3d6678ebf4953100925760dd870796ddc45fd5963000000004c53ff02aa7ed3011fdf667780000001eeb20ff31f20c04c0029b38028a3b4b4ee0aa13ffac27b656e92d6515b7151bc03af1f54f20758e692dd046534666df8e83463f894d08a20c1d65aea36a8f40058000000004c53ff02aa7ed3012618a41980000000b97b915b2d01007e57641495b7af8ce9d0ddca3c57221cfbef97980eee639e2202b8771528e198c7f459ea1912b475cf52bf0b6439b715ce1df2daf17659721dc30000000053ae98d50900";
	uint8_t tx_hex[875] = {'\0'};
	int ret = -1;
	ret = array_char_to_hex(1748, tx_char, 874, tx_hex);

	ret = init_tx(tx_hex, 874, &tx_st);
	if(ret != 0)
	{
		printf("INIT ERROR\n");
		return -1;
	}	


#endif



	uint64_t tx_hex_len = 0;
	ret = get_tx_hex_len(&tx_st, &tx_hex_len);
	if(-1 == ret)
	{
		printf("main():get_tx_hex_len() failed.\n");
		return -1;
	}
	
	printf("tx hex len:%ld\n", tx_hex_len);


#if 0
	uint8_t buffer1[262] = {'\0'};
	uint8_t buffer2[262] = {'\0'};


	ret = gen_preimage((uint8_t *)(&(tx_st.nversion)), tx_st.inputs_list, tx_st.inputs_counter, tx_st.outputs_list, tx_st.outputs_counter, (SPC_WITNESS *)(tx_st.witnesses_list), 0, (uint8_t *)(&(tx_st.lock_time)), buffer1, 262);
	if(ret != 0)
	{
		printf("GEN PRE IMAGE ERROR\n");
		free_tx(&tx_st);
		return -1;
	}	
	
	int i = 0;
	int j = 0;
	for(i = 0; i < 262; i++)
	{
		printf("%02x", buffer1[i]);
	}
	printf("\n");
#if 1
	ret = gen_preimage((uint8_t *)(&(tx_st.nversion)), tx_st.inputs_list, tx_st.inputs_counter, tx_st.outputs_list, tx_st.outputs_counter, (SPC_WITNESS *)((uint8_t *)(tx_st.witnesses_list)+sizeof(SPC_WITNESS)), 1, (uint8_t *)(&(tx_st.lock_time)), buffer2, 262);
	if(ret != 0)
	{
		printf("GEN PRE IMAGE ERROR\n");
		free_tx(&tx_st);
		return -1;
	}	

	for(i = 0; i < 262; i++)
	{
		printf("%02x", buffer2[i]);
	}
	printf("\n");
#endif	

	uint8_t pubs_list[33*3] = {'\0'};
	uint8_t path1s_list[4*3] = {'\0'};
	uint8_t path2s_list[4*3] = {'\0'};

	ret = get_publist_and_paths((SPC_WITNESS *)(tx_st.witnesses_list + sizeof(SPC_WITNESS)), pubs_list, path1s_list, path2s_list);
	if(ret != 0)
	{
		printf("GET PUBS LIST AND PATHS ERROR\n");
		free_tx(&tx_st);
		return -1;
	}


	printf("pubs list:\n");
	for(i = 0; i < 3; i ++)
	{
		for(j = 0; j < 33; j ++)
		{
			printf("%02x", pubs_list[i*33 + j]);
		}
		printf("\n");
	}

	uint32_t path1_get;
	printf("path1s list:\n");
	for(i = 0; i < 3; i ++)
	{
		memcpy(&path1_get, path1s_list+i*4, sizeof(uint32_t));
		printf("path1:%d:%d\n", i, path1_get);
	}

	uint32_t path2_get;
	printf("pubs list:\n");
	for(i = 0; i < 3; i ++)
	{
		memcpy(&path2_get, path2s_list+i*4, sizeof(uint32_t));
		printf("path2:%d:%d\n", i, path2_get);
	}
#endif 


#if 0
	//mktx
	uint8_t xprv_str[113] = 
	"ZprvAjvuvS3mTHjfeWU3PjZmVqB5oPxkzG8aMeqXjaJWcp8uSpp3BBqaH7xXi31XXg8rZ3arJu9iL963wPAuimZbKVRCbxmFNCChT17aL84GTXh";
	uint8_t xpub_str[113] = 
	"Zpub6xvGKwafHfHxrzYWVm6mry7pMRoFPirRism8Xxi8B9ftKd9Bij9ppvH1ZJFnJvJdCLnfc2RAcVCVfTjMw75MEZpFokaEfAeKjiXeVqGkp8a";
#endif 

#if 0
	//mktx
	uint8_t xprv_str[113] = 
	"ZprvAjV8fQabzDwjsG87rBmEyk87HPVpVs1FAYy7hKBoeinBqaFwgUHBsdaZjQfs9p3TjuWDo5XBsxMaoWmgULpDgnLaAK2RPYjJ87xA3HUryq5";
	uint8_t xpub_str[113] = 
	"Zpub6xUV4v7VpbW35kCaxDJFLt4qqRLJuKj6XmtiVhbRD4KAiNb6E1bSRRu3ahdnTw5TdfjAidoMnbSXCs9AnCpjNQ8e9U4ywK7f6TNNvqifT5t";



#endif

#if 1
	uint8_t xprv_str[113] = 
	"ZprvAjhMF4HJWpxypMxq6N77AnUkU6MU44ZusXTH96NzFViqEZcqEiL7iSLDWTQ7V2o4xKKwnFoxsw9iRYhNTtheMbiaWANVKRkiczpZMviWWqF";
	uint8_t xpub_str[113] = 
	"Zpub6xgheZpCMCXH2r3JCPe7XvRV28BxTXHmEkNswUnboqFp7MwynFeNGEehMk6GcayF8RVZ9YRA6KUUtwjoFDfnQCeBumrDFyeYHnqfNfFnEgy";



#endif


#if 0
	uint32_t path1_32 = 0;
	uint32_t path2_32 = 0;
	uint8_t order = 3;
	//memcpy(&path1_32, path1s_list+2*sizeof(uint32_t), sizeof(uint32_t));
	//memcpy(&path2_32, path2s_list+2*sizeof(uint32_t), sizeof(uint32_t));
	
	//ret = get_publist_and_paths((SPC_WITNESS *)(tx_st.witnesses_list + sizeof(SPC_WITNESS)), pubs_list, path1s_list, path2s_list);
	ret = get_order_and_paths(xpub_str, 113, (SPC_WITNESS *)(tx_st.witnesses_list + sizeof(SPC_WITNESS)), &order, &path1_32, &path2_32);
	if(ret == -1)
	{
		printf("get_order_and_paths():failed.\n");
		free_tx(&tx_st);
		return -1;
	}

	
	printf("path1:%d\n", path1_32);
	printf("path2:%d\n", path2_32);
	printf("order:%d\n", order);

	HDNode node_st;
	ret = get_node_from_xprv_double_ckd(xprv_str, 113, path1_32, path2_32, &node_st);
	if(ret != 0)	
	{
		printf("GET PRIVKEY ERROR\n");
		free_tx(&tx_st);
		return -1;
		
	}

	uint8_t pub_get_from_d_ckd[33] = {'\0'};
	ret = get_pubkey_from_xpub_base58_double_ckd(xpub_str, 113, path1_32, path2_32, pub_get_from_d_ckd, 33);
	if(ret != 0)
	{
		printf("get_pubkey_from_xpub_base58_double_ckd():failed.\n");
		free_tx(&tx_st);
		return -1;
	}
	printf("pub get from d ckd:\n");
	for(i = 0; i < 33; i++)
	{
		printf("%02x", pub_get_from_d_ckd[i]);
	}
	printf("\n");


	uint8_t sig_der[73] = {'\0'};
	uint32_t sig_len;
	ret = gen_sig(&node_st, buffer2, 262, sig_der, &sig_len);
	if(ret == -1)
	{
		printf("Failed to generate sig in DER form.\n");
		free_tx(&tx_st);
		return -1; 
	}

	printf("main():get sig_der len:%d\n", sig_len);
	printf("main():get sig_der:\n");
	for(i = 0; i < 72; i++)
	{
		printf("%02x", sig_der[i]);
	}
	printf("\n");

#endif
	ret = sign_tx(xpub_str, 113, xprv_str, 113, &tx_st);
	if(-1 == ret)
	{
		free_tx(&tx_st);
		printf("main():sign_tx failed.\n");
		return -1;
	}	

	SPC_WITNESS *wit_ptr = NULL;
	OP_ITEM *item_ptr = NULL;
	uint64_t op_items_num = 0;
	uint64_t op_item_len = 0;
	uint64_t inputs_num = tx_st.inputs_counter;
	uint64_t i = 0;
	uint64_t j = 0;
	uint64_t k = 0;
	for(i = 0; i < inputs_num; i ++)
	{
		wit_ptr = (SPC_WITNESS *)(tx_st.witnesses_list + i*sizeof(SPC_WITNESS));
		op_items_num = wit_ptr->op_items_counter;
		for(j = 0; j < op_items_num; j ++)
		{
			item_ptr = (OP_ITEM *)(wit_ptr->op_items_list + j*sizeof(OP_ITEM));
			op_item_len = item_ptr->op_item_len;
			printf("wit[%ld]:op_item[%ld]:len:%ld\n", i, j, op_item_len);
			printf("wit[%ld]:op_item[%ld]:\n", i, j);
			for(k = 0; k < op_item_len; k ++)
			{
				printf("%02x", (item_ptr->op_item)[k]);
			}
			printf("\n");
		}
	}

//serialize part
	uint64_t inputs_len = 0;
	ret = get_inputs_hex_len(tx_st.inputs_list, tx_st.inputs_counter, &inputs_len);
	if(-1 == ret)
	{
		printf("main():get_inputs_hex_len() failed.\n");
		free_tx(&tx_st);
		return -1;
	}
	
	printf("inputs hex len:%ld\n", inputs_len);
	
	uint8_t *inputs_buf = NULL;
	size_t inputs_len_size_t = inputs_len;
	inputs_buf = malloc(inputs_len_size_t);
	if(NULL == inputs_buf)
	{
		printf("main():failed to malloc for inputs.\n");
		free_tx(&tx_st);
		return -1;
	} 

	uint64_t inputs_hex_len = 0;
	ret = serialize_inputs(tx_st.inputs_list, tx_st.inputs_counter, inputs_buf, &inputs_hex_len);
	if(-1 == ret)
	{
		printf("main():failed to serialize inputs.\n");
		free_tx(&tx_st);
		free(inputs_buf);
		return -1;
	}
	printf("inputs_serialized_hex:\n");
	for(i = 0; i < inputs_hex_len; i++)
	{
		printf("%02x", inputs_buf[i]);
	}
	printf("\n");
	free(inputs_buf);

	uint64_t outputs_len = 0;
	ret = get_outputs_hex_len(tx_st.outputs_list, tx_st.outputs_counter, &outputs_len);
	if(-1 == ret)
	{
		printf("main():get_outputs_hex_len() failed.\n");
		return -1;
	}
	
	printf("outputs hex len:%ld\n", outputs_len);

	uint8_t *outputs_buf = NULL;
	size_t outputs_len_size_t = outputs_len;
	outputs_buf = malloc(outputs_len_size_t);
	if(NULL == outputs_buf)
	{
		printf("main():failed to malloc for outputs.\n");
		free_tx(&tx_st);
		return -1;
	} 

	uint64_t outputs_hex_len = 0;
	ret = serialize_outputs(tx_st.outputs_list, tx_st.outputs_counter, outputs_buf, &outputs_hex_len);
	if(-1 == ret)
	{
		printf("main():failed to serialize outputs.\n");
		free_tx(&tx_st);
		free(outputs_buf);
		return -1;
	}
	printf("outputs_serialized_hex:\n");
	for(i = 0; i < outputs_hex_len; i++)
	{
		printf("%02x", outputs_buf[i]);
	}
	printf("\n");
	free(outputs_buf);


	
	uint64_t witnesses_len = 0;
	ret = get_witnesses_hex_len(tx_st.witnesses_list, tx_st.witnesses_counter, &witnesses_len);
	if(-1 == ret)
	{
		printf("main():get_witnesses_hex_len() failed.\n");
		return -1;
	}
	
	printf("witnesses hex len:%ld\n", witnesses_len);
	
	uint8_t *witnesses_buf = NULL;
	size_t witnesses_len_size_t = witnesses_len;
	witnesses_buf = malloc(witnesses_len_size_t);
	if(NULL == witnesses_buf)
	{
		printf("main():failed to malloc for witnesses.\n");
		free_tx(&tx_st);
		return -1;
	} 

	uint64_t witnesses_hex_len = 0;
	ret = serialize_witnesses(tx_st.witnesses_list, tx_st.witnesses_counter, witnesses_buf, &witnesses_hex_len);
	if(-1 == ret)
	{
		printf("main():failed to serialize witnesses.\n");
		free_tx(&tx_st);
		free(witnesses_buf);
		return -1;
	}
	printf("witnesses_serialized_hex:\n");
	for(i = 0; i < witnesses_hex_len; i++)
	{
		printf("%02x", witnesses_buf[i]);
	}
	printf("\n");
	free(witnesses_buf);

	tx_hex_len = 0;
	ret = get_tx_hex_len(&tx_st, &tx_hex_len);
	if(-1 == ret)
	{
		printf("main():get_tx_hex_len() failed.\n");
		return -1;
	}
	
	printf("tx hex len:%ld\n", tx_hex_len);

	uint8_t *tx_hex_buf = NULL;
	size_t tx_hex_len_size_t = tx_hex_len;
	tx_hex_buf = malloc(tx_hex_len_size_t);
	if(NULL == tx_hex_buf)
	{
		printf("main():failed to malloc for tx hex.\n");
		free_tx(&tx_st);
		return -1;
	} 

	uint64_t tx_hex_len_serialize = 0;
	ret = serialize_tx(&tx_st, tx_hex_buf, &tx_hex_len_serialize);
	if(-1 == ret)
	{
		printf("main():failed to serialize tx.\n");
		free_tx(&tx_st);
		free(tx_hex_buf);
		return -1;
	}
	
	printf("tx hex len get:%ld\n", tx_hex_len);
	printf("tx hex len ser:%ld\n", tx_hex_len_serialize);

	printf("tx_serialized_hex:\n");
	for(i = 0; i < tx_hex_len_serialize; i++)
	{
		printf("%02x", tx_hex_buf[i]);
	}
	printf("\n");
	free(tx_hex_buf);

	
	free_tx(&tx_st);


	return 0;
}
