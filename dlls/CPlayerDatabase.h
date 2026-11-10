#ifndef CPLAYERDATABASE_H
#define CPLAYERDATABASE_H

#include "donid.h"
#include "CMutex.h"

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
	static size_t OnData( char *data, size_t size, size_t count, void *context );
#ifdef _WIN32
	static DWORD WINAPI ThreadEntry( void *context );

	HANDLE m_hThread;
#else
	static void *ThreadEntry( void *context );

	pthread_t m_Thread;
#endif

	CMutex  m_Mutex;
	bool    m_bStarted;
	bool    m_bFinished;
	bool    m_bCancelled;

	char       m_szAuthId[EXTRAS_AUTHID_LENGTH + 1];
	char       m_szIpAddress[EXTRAS_IP_ADDRESS_LENGTH + 1];
	char       m_szUrl[256];
	long long  m_iTimestamp;
	float      m_flQueryTime;

	extrasinfo_s  m_Result;
	char          m_szError[256];

	char    *m_pResponse;
	size_t  m_uiResponseLength;

	extrasinfo_s	*m_pQueryResult;
	int				m_iRequestCode;
};

#endif
