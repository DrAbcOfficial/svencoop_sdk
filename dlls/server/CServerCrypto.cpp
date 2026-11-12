#include "extdll.h"
#include "enginecallback.h"
#include "server/CServerCrypto.h"
#include "../../external/cryptopp/filters.h"
#include <string>

void CServerCrypto::Digest( unsigned char output[SERVER_CRYPTO_DIGEST_SIZE], const unsigned char *data, unsigned int length )
{
	m_Digester.CalculateDigest( output, data, length );
}

void CServerCrypto::ClearCryptData( servercryptdata_t &data )
{
	delete[] data.iv;
	delete[] data.encryptedData;
	delete[] data.encryptedDigest;
	memset( &data, 0, sizeof( data ) );
}

bool CServerCrypto::Encrypt( servercryptdata_t &data, const unsigned char *plain, unsigned int length,
                             const unsigned char *key, unsigned int keyLength )
{
	ClearCryptData( data );

	if ( !plain || !length || !key || !m_Encrypter.IsValidKeyLength( keyLength ) )
		return false;

	try
	{
		data.plainLength = length;
		data.ivLength = m_Encrypter.DefaultIVLength();
		data.iv = new unsigned char[data.ivLength];
		m_Random.GenerateBlock( data.iv, data.ivLength );
		m_Encrypter.SetKeyWithIV( key, keyLength, data.iv, data.ivLength );

		std::string encrypted;
		CryptoPP::AuthenticatedEncryptionFilter filter( m_Encrypter, new CryptoPP::StringSink( encrypted ), false, SERVER_CRYPTO_TAG_SIZE );

		filter.Put( plain, length );
		filter.MessageEnd();
		data.encryptedDataLength = (unsigned int) encrypted.size();
		data.encryptedData = new unsigned char[data.encryptedDataLength];
		memcpy( data.encryptedData, encrypted.data(), data.encryptedDataLength );
		data.encryptedDigest = new unsigned char[SERVER_CRYPTO_DIGEST_SIZE];
		Digest( data.encryptedDigest, data.encryptedData, data.encryptedDataLength );
		return true;
	}
	catch ( const std::exception &error )
	{
		ALERT( at_error, "Encrypt processing error: %s\n", error.what() );
		ClearCryptData( data );
		return false;
	}
}

void CServerCrypto::WriteCryptData( const servercryptdata_t &data )
{
	WRITE_LONG( data.plainLength );
	WRITE_LONG( data.ivLength );

	for ( unsigned int i = 0; i < data.ivLength; i++ )
		WRITE_BYTE( data.iv[i] );

	WRITE_LONG( data.encryptedDataLength );

	for ( unsigned int i = 0; i < data.encryptedDataLength; i++ )
		WRITE_BYTE( data.encryptedData[i] );

	WRITE_LONG( SERVER_CRYPTO_DIGEST_SIZE );

	for ( unsigned int i = 0; i < SERVER_CRYPTO_DIGEST_SIZE; i++ )
		WRITE_BYTE( data.encryptedDigest[i] );
}
