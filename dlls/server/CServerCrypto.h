#ifndef CSERVERCRYPTO_H
#define CSERVERCRYPTO_H

#include "../../external/cryptopp/gcm.h"
#include "../../external/cryptopp/aes.h"
#include "../../external/cryptopp/sha.h"
#include "../../external/cryptopp/osrng.h"

enum
{
	SERVER_CRYPTO_DIGEST_SIZE = 32,
	SERVER_CRYPTO_TAG_SIZE = 12
};

struct servercryptdata_t
{
	unsigned int   plainLength;
	unsigned char  *iv;
	unsigned int   ivLength;
	unsigned char  *encryptedData;
	unsigned int   encryptedDataLength;
	unsigned char  *encryptedDigest;
};

class CServerCrypto
{
public:
	void Digest( unsigned char output[SERVER_CRYPTO_DIGEST_SIZE], const unsigned char *data, unsigned int length );
	bool Encrypt( servercryptdata_t &data, const unsigned char *plain, unsigned int length,
	              const unsigned char *key, unsigned int keyLength );
	void ClearCryptData( servercryptdata_t &data );
	void WriteCryptData( const servercryptdata_t &data );

private:
	CryptoPP::AutoSeededRandomPool            m_Random;
	CryptoPP::GCM<CryptoPP::AES>::Encryption  m_Encrypter;
	CryptoPP::SHA256                          m_Digester;
};

#endif
