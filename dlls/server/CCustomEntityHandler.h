#ifndef CCUSTOMENTITYHANDLER_H
#define CCUSTOMENTITYHANDLER_H

#include <stdarg.h>

class CBaseEntity;
class CASDocumentation;
class CASMethod;
class CASObjectInstance;
class asIScriptFunction;

class CCustomEntityHandler
{
public:
	CCustomEntityHandler(CBaseEntity *entity);
	~CCustomEntityHandler();

	bool MethodExists(const char *declaration) const;
	CASObjectInstance *GetScriptInstance(void) const { return m_pScriptInstance; }
	void SetScriptInstance(CASObjectInstance *instance);
	bool CallMethod(bool optional, const char *declaration, ...);
	bool CallMethod(void *returnValue, bool optional, const char *declaration, ...);
	bool CallMethod(const char *declaration, va_list arguments);
	bool CallMethod(CASMethod *&function, const char *declaration, va_list arguments);
	bool CallMethod(CASMethod *&function, const char *declaration, ...);
	bool CallThink(void);
	bool CallTouch(CBaseEntity *other);
	bool CallBlocked(CBaseEntity *other);
	bool CallUse(CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value);

	void SetThinkFunction(asIScriptFunction *function);
	void SetTouchFunction(asIScriptFunction *function);
	void SetBlockedFunction(asIScriptFunction *function);
	void SetUseFunction(asIScriptFunction *function);

	static void ScheduleForRemoval(CCustomEntityHandler *handler);
	static void Clear(void);
	static void Think(void);

private:
	struct CachedFunction_t
	{
		CachedFunction_t	*m_pNext;
		CASMethod			*m_pMethod;
	};

	struct Removal_t
	{
		Removal_t				*m_pNext;
		CCustomEntityHandler	*m_pHandler;
	};

	CASMethod *GetFunctionFromCache(asIScriptFunction *function) const;
	void AddFunctionToCache(CASMethod *function);
	void SetFunction(CASMethod *&destination, asIScriptFunction *function, const char *name);
	void DeleteFunctions(void);

	CBaseEntity			*m_pEntity;
	CASObjectInstance	*m_pScriptInstance;
	CASMethod			*m_pThinkFunction;
	CASMethod			*m_pTouchFunction;
	CASMethod			*m_pBlockedFunction;
	CASMethod			*m_pUseFunction;
	CachedFunction_t	*m_pFunctionCacheHead;

	static Removal_t *m_pToRemoveHead;
};

void RegisterSCScriptCustomEntityCallbackHandler(CASDocumentation *documentation);

#endif
