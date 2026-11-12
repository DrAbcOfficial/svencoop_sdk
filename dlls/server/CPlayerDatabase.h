#ifndef CPLAYERDATABASE_H
#define CPLAYERDATABASE_H

#include "server/donid.h"

#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

class CPlayerDatabase
{
public:
	CPlayerDatabase( const char *authId, const char *ipAddress, long long timestamp,
	                 float queryTime, const char *url = NULL );
	~CPlayerDatabase();

	static bool Initialize();
	static void Shutdown();
	bool Start();
	void Cancel();
	void Join();
	bool IsFinished();
	const extrasinfo_s &GetResult() const;
	const char *GetError() const;
	void GetPlayerExtras( extrasinfo_s *extras );

private:
	CPlayerDatabase( const CPlayerDatabase & );
	CPlayerDatabase &operator=( const CPlayerDatabase & );

	bool PerformHttpRequest();
	void DecodeServerResponse();
	bool IsCancelled();
	void ThreadEntry( std::stop_token stopToken );
	static size_t OnData( char *data, size_t size, size_t count, void *context );

	std::mutex       m_Mutex;
	std::stop_source m_StopSource;
	std::jthread     m_Thread;

	bool    m_bStarted;
	bool    m_bFinished;

	char       m_szAuthId[EXTRAS_AUTHID_LENGTH + 1];
	char       m_szIpAddress[EXTRAS_IP_ADDRESS_LENGTH + 1];
	char       m_szUrl[512];
	long long  m_iTimestamp;
	float      m_flQueryTime;

	extrasinfo_s  m_Result;
	char          m_szError[256];

	std::vector<char>  m_Response;

	extrasinfo_s	*m_pQueryResult;
	int				m_iRequestCode;
};

#endif
