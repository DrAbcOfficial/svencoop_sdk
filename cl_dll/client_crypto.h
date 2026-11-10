#ifndef CLIENT_CRYPTO_H
#define CLIENT_CRYPTO_H

#include "../external/cryptopp/osrng.h"
#include "../external/cryptopp/aes.h"
#include "../external/cryptopp/gcm.h"
#include "../external/cryptopp/sha.h"
#include "../external/cryptopp/hex.h"

struct clientcryptdata_t
{
	unsigned int	plainLength;
	unsigned char	*iv;
	unsigned int	ivLength;
	unsigned char	*encryptedData;
	unsigned int	encryptedDataLength;
	unsigned char	*encryptedDigest;
};

class CClientCrypto
{
public:
	unsigned int Random(unsigned int minimum = 0, unsigned int maximum = ~0U);
	unsigned int DigesterSize() const;
	bool Digest(unsigned char **output, const unsigned char *data, unsigned int length);
	void ClearCryptData(clientcryptdata_t *data);
	bool SetCryptData(clientcryptdata_t *data, unsigned int plainLength, const unsigned char *iv, unsigned int ivLength,
		const unsigned char *encryptedData, unsigned int encryptedDataLength, const unsigned char *encryptedDigest);
	void ReadCryptData(clientcryptdata_t *data);
	bool Decrypt(const clientcryptdata_t *data, unsigned char **output, const unsigned char *key, unsigned int keyLength);

private:
	CryptoPP::AutoSeededRandomPool			m_Random;
	CryptoPP::GCM<CryptoPP::AES>::Encryption	m_Encrypter;
	CryptoPP::GCM<CryptoPP::AES>::Decryption	m_Decrypter;
	CryptoPP::SHA256							m_Digester;
	CryptoPP::HexEncoder						m_HexEncoder;
	CryptoPP::HexDecoder						m_HexDecoder;
};

#endif
