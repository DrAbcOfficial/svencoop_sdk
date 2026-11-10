#include "client_crypto.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <stdexcept>

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_TeamFortressViewport.h"

void CryptCheck(bool condition, const char *message)
{
	if (!condition)
		throw std::runtime_error(message && *message ? message : "Assert failed.");
}

unsigned int CClientCrypto::Random(unsigned int minimum, unsigned int maximum)
{
	return m_Random.GenerateWord32(minimum, maximum);
}

unsigned int CClientCrypto::DigesterSize() const
{
	return m_Digester.DigestSize();
}

bool CClientCrypto::Digest(unsigned char **output, const unsigned char *data, unsigned int length)
{
	m_Digester.CalculateDigest(*output, data, length);
	return output != NULL;
}

void CClientCrypto::ClearCryptData(clientcryptdata_t *data)
{
	if (!data)
		return;

	data->plainLength = 0;
	data->ivLength = 0;
	if (data->iv)
	{
		delete[] data->iv;
		data->iv = NULL;
	}

	data->encryptedDataLength = 0;
	if (data->encryptedData)
	{
		delete[] data->encryptedData;
		data->encryptedData = NULL;
	}

	if (data->encryptedDigest)
	{
		delete[] data->encryptedDigest;
		data->encryptedDigest = NULL;
	}
}

bool CClientCrypto::SetCryptData(clientcryptdata_t *data, unsigned int plainLength, const unsigned char *iv, unsigned int ivLength,
	const unsigned char *encryptedData, unsigned int encryptedDataLength, const unsigned char *encryptedDigest)
{
	unsigned char *digest = NULL;
	bool result = false;

	try
	{
		CryptCheck(data != NULL, "Output data structure is null.");
		CryptCheck(plainLength >= 1, "Plain length is zero.");
		CryptCheck(iv != NULL, "IV pointer is null.");
		CryptCheck(ivLength >= 1, "IV length is zero.");
		CryptCheck(encryptedData != NULL, "Encrypted data pointer is null.");
		CryptCheck(encryptedDataLength >= 1, "Encrypted data length is zero.");
		ClearCryptData(data);

		data->plainLength = plainLength;
		data->iv = new unsigned char[ivLength];
		data->ivLength = ivLength;
		data->encryptedData = new unsigned char[encryptedDataLength];
		data->encryptedDataLength = encryptedDataLength;
		data->encryptedDigest = new unsigned char[DigesterSize()];
		memmove(data->iv, iv, ivLength);
		memmove(data->encryptedData, encryptedData, encryptedDataLength);

		if (encryptedDigest)
		{
			digest = new unsigned char[DigesterSize()];
			m_Digester.CalculateDigest(digest, encryptedData, encryptedDataLength);
			CryptCheck(!memcmp(digest, encryptedDigest, DigesterSize()), "Encrpyted digest mismatch.");
			memmove(data->encryptedDigest, encryptedDigest, DigesterSize());
		}
		else
		{
			m_Digester.CalculateDigest(data->encryptedDigest, encryptedData, encryptedDataLength);
		}

		result = true;
	}
	catch (const std::exception &error)
	{
		if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
			gEngfuncs.Con_Printf("Set crypt data error: %s\n", error.what());
	}

	delete[] digest;
	return result;
}

void CClientCrypto::ReadCryptData(clientcryptdata_t *data)
{
	CryptCheck(data != NULL, "Data structure is null.");

	unsigned char *iv = NULL;
	unsigned char *encryptedData = NULL;
	unsigned char *encryptedDigest = NULL;

	try
	{
		unsigned int plainLength = READ_LONG();

		CryptCheck(plainLength >= 1, "Plain data length is zero.");

		unsigned int ivLength = READ_LONG();

		CryptCheck(ivLength >= 1, "IV length is zero.");
		iv = new unsigned char[ivLength];
		for (unsigned int i = 0; i < ivLength; i++)
			iv[i] = (unsigned char)READ_BYTE();

		unsigned int encryptedDataLength = READ_LONG();

		CryptCheck(encryptedDataLength >= 1, "Encrypted data length is zero.");
		encryptedData = new unsigned char[encryptedDataLength];
		for (unsigned int i = 0; i < encryptedDataLength; i++)
			encryptedData[i] = (unsigned char)READ_BYTE();

		unsigned int encryptedDigestLength = READ_LONG();

		CryptCheck(encryptedDigestLength >= 1, "Encrypted digest length is zero.");
		CryptCheck(encryptedDigestLength == DigesterSize(), "Encrypted digest length unexpected.");
		encryptedDigest = new unsigned char[encryptedDigestLength];
		for (unsigned int i = 0; i < encryptedDigestLength; i++)
			encryptedDigest[i] = (unsigned char)READ_BYTE();

		CryptCheck(SetCryptData(data, plainLength, iv, ivLength, encryptedData, encryptedDataLength, encryptedDigest), "Crypt object error.");
	}
	catch (const std::exception &)
	{
	}

	delete[] iv;
	delete[] encryptedData;
	delete[] encryptedDigest;
}

bool CClientCrypto::Decrypt(const clientcryptdata_t *data, unsigned char **output, const unsigned char *key, unsigned int keyLength)
{
	unsigned char *digest = NULL;
	unsigned char *first = NULL;
	unsigned char *remaining = NULL;
	bool result = false;

	try
	{
		CryptCheck(data != NULL, "Data structure is null.");
		CryptCheck(data->plainLength >= 1, "Plain length is zero.");
		CryptCheck(data->iv != NULL, "IV pointer is null.");
		CryptCheck(data->ivLength >= 1, "IV length is zero.");
		CryptCheck(data->encryptedData != NULL, "Encrypted data pointer is null.");
		CryptCheck(data->encryptedDataLength >= 1, "Encrypted data length is zero.");
		CryptCheck(data->encryptedDigest != NULL, "Encrypted digest pointer is null.");
		CryptCheck(key != NULL, "Key pointer is null.");
		CryptCheck(m_Decrypter.IsValidKeyLength(keyLength), "Key length invalid.");
		m_Decrypter.SetKeyWithIV(key, keyLength, data->iv, data->ivLength);

		digest = new unsigned char[DigesterSize()];
		m_Digester.CalculateDigest(digest, data->encryptedData, data->encryptedDataLength);
		CryptCheck(!memcmp(digest, data->encryptedDigest, DigesterSize()), "Encrpyted digest mismatch.");

		unsigned int remainingLength = data->encryptedDataLength - 12;

		first = new unsigned char[12];
		remaining = new unsigned char[remainingLength];
		memmove(first, data->encryptedData, 12);
		memmove(remaining, data->encryptedData + 12, remainingLength);

		std::string plain;
		CryptoPP::AuthenticatedDecryptionFilter filter(m_Decrypter, new CryptoPP::StringSink(plain),
			CryptoPP::AuthenticatedDecryptionFilter::THROW_EXCEPTION, 12, CryptoPP::BlockPaddingSchemeDef::DEFAULT_PADDING);

		filter.ChannelPut(CryptoPP::DEFAULT_CHANNEL, first, 12);
		filter.ChannelPut(CryptoPP::DEFAULT_CHANNEL, remaining, remainingLength);
		filter.ChannelMessageEnd(CryptoPP::DEFAULT_CHANNEL);
		CryptCheck(filter.GetLastResult(), "Decrypter integrity failed.");

		*output = new unsigned char[plain.size()];
		memset(*output, 0, plain.size());
		memmove(*output, plain.data(), plain.size());
		result = true;
	}
	catch (const CryptoPP::Exception &error)
	{
		if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
			gEngfuncs.Con_Printf("Decrypt processing error: %s\n", error.GetWhat().c_str());
	}
	catch (const std::exception &error)
	{
		if (gEngfuncs.pfnGetCvarFloat("developer") >= 1)
			gEngfuncs.Con_Printf("Decrypt error: %s\n", error.what());
	}

	delete[] digest;
	delete[] first;
	delete[] remaining;
	return result;
}

void CryptMapName(const char *level, char *name)
{
	if (!level)
		return;

	char buffer[44];

	strncpy(buffer, level, 41);
	buffer[41] = 0;

	char *separator = strchr(buffer, '.');

	if (separator)
		*separator = 0;

	separator = strchr(buffer, '/');
	if (separator)
		memmove(buffer, separator + 1, strlen(separator) + 1);

	memset(buffer + strlen(buffer), 0, 42 - strlen(buffer));
	strncpy(name, buffer, 31);
	name[31] = 0;
	memset(name + strlen(name), 0, 33 - strlen(name));
}

unsigned char *CHud::GetCryptKey(unsigned int index, bool force)
{
	if (index >= 2)
		return NULL;

	if (!force && m_pCryptKeys[index])
		return m_pCryptKeys[index];

	char buffer[2048] = {};
	unsigned long long currentTime = (unsigned long long)time(NULL);

	if (index == 1)
	{
		char mapName[128] = {};

		CryptMapName(gEngfuncs.pfnGetLevelName(), mapName);
		if (!gViewPort || !gViewPort->m_szServerHostname[0] || m_iClientIndex <= 0 || m_iClientUserID <= 0
			|| !mapName[0] || !m_szClientAuthID[0])
			return NULL;

		snprintf(buffer, sizeof(buffer), "%u\n%s\n%u\n%s\n%s", m_iClientIndex, gViewPort->m_szServerHostname,
			m_iClientUserID, mapName, m_szClientAuthID);
	}
	else
	{
		m_Crypto.Random();
		snprintf(buffer, sizeof(buffer), "%u\n%s\n%u\n%u", 0, gEngfuncs.pfnGetCvarString("name"),
			(unsigned int)currentTime, (unsigned int)(currentTime >> 32));
	}

	buffer[sizeof(buffer) - 1] = 0;
	if (!buffer[0])
		return NULL;

	m_pCryptKeys[index] = new unsigned char[m_Crypto.DigesterSize()];
	m_Crypto.Digest(&m_pCryptKeys[index], (const unsigned char *)buffer, strlen(buffer));
	return m_pCryptKeys[index];
}

void CHud::ReleaseCryptKey(unsigned int index)
{
	if (index < 2 && m_pCryptKeys[index])
	{
		delete[] m_pCryptKeys[index];
		m_pCryptKeys[index] = NULL;
	}
}

int CHud::MsgFunc_ClServerInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	m_iClientIndex = (unsigned char)READ_BYTE();
	m_iClientUserID = READ_LONG();

	char authID[44] = {};

	strncpy(authID, READ_STRING(), sizeof(authID) - 1);
	if (authID[0])
	{
		strncpy(m_szClientAuthID, authID, sizeof(m_szClientAuthID) - 1);
		m_szClientAuthID[sizeof(m_szClientAuthID) - 1] = 0;
	}

	return m_iClientIndex >= 1 && m_iClientUserID >= 1 && m_szClientAuthID[0] && GetCryptKey(1, true) != NULL;
}

int CHud::MsgFunc_ClExtrasInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	if (!gViewPort)
		return 0;

	clientcryptdata_t data = {};
	unsigned char *plain = NULL;
	char *message = NULL;
	bool result = false;

	try
	{
		m_Crypto.ReadCryptData(&data);

		unsigned char *key = GetCryptKey(1);

		CryptCheck(key != NULL, "Key pointer is null.");
		CryptCheck(m_Crypto.Decrypt(&data, &plain, key, m_Crypto.DigesterSize()) && plain != NULL, "Decrypt error.");
		message = new char[data.plainLength + 1];
		CryptCheck(message != NULL, "Message pointer is null.");
		memset(message, 0, data.plainLength + 1);
		memmove(message, plain, data.plainLength);
		CryptCheck(message[0] != 0, "Message empty.");

		int player = 0;
		char authID[45] = {};
		char name[33] = {};
		char description[129] = {};
		int level = -3;

		CryptCheck(sscanf(message, "%d\n%44s\n%*[\"]%32[^\"]%*[\"]\n%*[\"]%128[^\"]%*[\"]\n%d",
			&player, authID, name, description, &level) == 5, "Segments mismatch.");
		CryptCheck((unsigned int)(player - 1) <= 31, "Player invalid.");
		CryptCheck((unsigned int)(level + 1) <= 7, "Level invalid.");
		g_PlayerExtraInfo[player].adminlevel = (short)level;
		result = true;
	}
	catch (const std::exception &)
	{
	}

	delete[] plain;
	delete[] message;
	return result;
}
