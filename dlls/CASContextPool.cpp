#include "extdll.h"
#include "util.h"
#include "CASContextPool.h"
#include "CASServerManager.h"
#include "CASDebugger.h"
#include "CASLog.h"

const float CASContextPool::DEFAULT_CHECK_INTERVAL_TIME = 300.0f;
const float CASContextPool::DISABLED_CHECK_TIME = -1.0f;

CASContextPool::CASContextPool()
	: m_pContexts(NULL), m_uiMaximum(DEFAULT_MAXIMUM_CONTEXTS),
	  m_uiTolerance(DEFAULT_CONTEXT_TOLERANCE), m_uiFreeCount(0), m_uiTotalCount(0),
	  m_uiActiveCount(0), m_uiHighWater(0), m_flNextCheckTime(DISABLED_CHECK_TIME),
	  m_flCheckInterval(DEFAULT_CHECK_INTERVAL_TIME)
{
}

CASContextPool::~CASContextPool()
{
	ClearAll();
}

CASContextPool *CASContextPool::GetInstance(void)
{
	static CASContextPool *instance;

	if (!instance)
		instance = new CASContextPool;

	return instance;
}

void CASContextPool::SetLimits(unsigned int maximum, unsigned int tolerance)
{
	if (!maximum && !tolerance)
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
		                               "CASContextPool::SetLimits: maximum context count and tolerance are both 0!\n");

	m_uiMaximum = maximum;
	m_uiTolerance = maximum;
}

void CASContextPool::Initialize(void)
{
	m_flNextCheckTime = DISABLED_CHECK_TIME;
	Think(true);
}

float CASContextPool::GetTime(void) const
{
	return gpGlobals->time;
}

void CASContextPool::Think(bool force)
{
	float time = gpGlobals->time;

	if (!force && m_flNextCheckTime > time)
		return;

	if (m_uiFreeCount != m_uiTotalCount)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASContextPool::Think: %u contexts total, but %u contexts in list!\n",
		                             m_uiTotalCount, m_uiFreeCount);

	if (m_uiActiveCount)
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT,
		                             "CASContextPool::Think: %u contexts still in use!\n", m_uiActiveCount);

	if (m_uiTotalCount > m_uiMaximum)
	{
		if (m_uiTotalCount > m_uiMaximum + m_uiTolerance)
			RemoveContexts(m_uiTotalCount - m_uiTolerance - m_uiMaximum);

		unsigned int retained = m_uiHighWater - m_uiMaximum;

		if (retained >= m_uiTolerance)
			retained = m_uiTolerance;

		if (retained < m_uiTolerance)
			RemoveContexts(m_uiTotalCount - retained - m_uiMaximum);
	}

	m_uiHighWater = 0;
	m_flNextCheckTime = time + m_flCheckInterval;
}

asIScriptContext *CASContextPool::RequestContext(asIScriptEngine *engine)
{
	if (!engine)
		return NULL;

	asIScriptContext *context;

	if (m_uiFreeCount)
	{
		ContextNode_t *next = m_pContexts->m_pNext;

		context = m_pContexts->m_pContext;
		delete m_pContexts;
		m_uiFreeCount--;
		m_pContexts = next;
	}
	else
	{
		m_uiTotalCount++;
		context = engine->CreateContext();
	}

	m_uiActiveCount++;

	if (m_uiActiveCount > m_uiHighWater)
		m_uiHighWater = m_uiActiveCount;

	context->SetLineCallback(asMETHOD(CDebugger, LineCallback),
	                         CASServerManager::GetInstance()->m_pDebugger, asCALL_THISCALL);
	return context;
}

void CASContextPool::ReturnContext(asIScriptContext *context)
{
	if (!context)
		return;

	ContextNode_t *node = new ContextNode_t;

	node->m_pContext = context;
	node->m_pNext = m_pContexts;
	m_uiFreeCount++;
	m_uiActiveCount--;
	m_pContexts = node;
}

void CASContextPool::ClearAll(void)
{
	if (m_uiActiveCount)
		CASLog::GetInstance()->Warning(AS_LOG_DEFAULT,
		                               "CASContextPool::ClearAll: clearing context pool while there are %u contexts in use!\n",
		                               m_uiActiveCount);

	ContextNode_t *node = m_pContexts;

	while (node)
	{
		ContextNode_t *next = node->m_pNext;

		node->m_pContext->Release();
		delete node;
		node = next;
	}

	m_pContexts = NULL;
	m_uiFreeCount = 0;
	m_uiTotalCount = 0;
	m_uiActiveCount = 0;
	m_uiHighWater = 0;
}

void CASContextPool::RemoveContexts(unsigned int count)
{
	if (!count)
		return;

	unsigned int removed = 0;
	ContextNode_t *node = m_pContexts;

	while (node && removed < count)
	{
		ContextNode_t *next = node->m_pNext;

		node->m_pContext->Release();
		delete node;
		removed++;
		node = next;
	}

	m_uiTotalCount -= removed;
	m_uiFreeCount -= removed;
	m_pContexts = node;
}

asIScriptContext *ASRequestContext(asIScriptEngine *engine, void *parameter)
{
	return ((CASContextPool *)parameter)->RequestContext(engine);
}

void ASReturnContext(asIScriptEngine *engine, asIScriptContext *context, void *parameter)
{
	((CASContextPool *)parameter)->ReturnContext(context);
}
