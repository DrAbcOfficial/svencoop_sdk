#ifndef CASCONTEXTPOOL_H
#define CASCONTEXTPOOL_H

class asIScriptEngine;
class asIScriptContext;

class CASContextPool
{
public:
	CASContextPool();
	~CASContextPool();

	static CASContextPool *GetInstance(void);
	void SetLimits(unsigned int maximum, unsigned int tolerance);
	void Initialize(void);
	void Think(bool force);
	asIScriptContext *RequestContext(asIScriptEngine *engine);
	void ReturnContext(asIScriptContext *context);
	void ClearAll(void);

private:
	struct ContextNode_t
	{
		ContextNode_t    *m_pNext;
		asIScriptContext *m_pContext;
	};

	enum
	{
		DEFAULT_MAXIMUM_CONTEXTS = 2,
		DEFAULT_CONTEXT_TOLERANCE = 10
	};

	static const float DEFAULT_CHECK_INTERVAL_TIME;
	static const float DISABLED_CHECK_TIME;

	float GetTime(void) const;
	void RemoveContexts(unsigned int count);

	ContextNode_t *m_pContexts;
	unsigned int   m_uiMaximum;
	unsigned int   m_uiTolerance;
	unsigned int   m_uiFreeCount;
	unsigned int   m_uiTotalCount;
	unsigned int   m_uiActiveCount;
	unsigned int   m_uiHighWater;
	float          m_flNextCheckTime;
	float          m_flCheckInterval;
};

asIScriptContext *ASRequestContext(asIScriptEngine *engine, void *parameter);
void ASReturnContext(asIScriptEngine *engine, asIScriptContext *context, void *parameter);

#endif
