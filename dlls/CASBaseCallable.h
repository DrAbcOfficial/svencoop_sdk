#ifndef CASBASECALLABLE_H
#define CASBASECALLABLE_H

#include "CASBaseClass.h"
#include <angelscript.h>
#include <stdarg.h>

class CASModule;
class CASArguments;
class CASReturnValue;

class CASBaseCallable : public CASRefCountedBaseClass
{
public:
	CASBaseCallable(asIScriptFunction *function, CASModule *module);
	virtual ~CASBaseCallable();

	static unsigned int ComputeReservedSpace(const char *returnType, const char *arguments);
	static bool FormatFunction(char *buffer, unsigned int size, const char *returnType,
		const char *name, const char *arguments);

	void Release(void) const;
	asIScriptFunction *GetFunction(void) const;
	CASModule *GetModule(void) const;
	asIScriptContext *GetContext(void) const;
	bool Equals(const CASBaseCallable *other) const;
	bool IsCompatible(const CASBaseCallable *other) const;

	void Prepare(void);
	void Unprepare(void);
	bool Execute(bool unprepare);

	bool SetArgByte(unsigned int index, char value);
	bool SetArgWord(unsigned int index, short value);
	bool SetArgDWord(unsigned int index, long value);
	bool SetArgQWord(unsigned int index, long long value);
	bool SetArgFloat(unsigned int index, float value);
	bool SetArgDouble(unsigned int index, double value);
	bool SetArgAddress(unsigned int index, void *value);
	bool SetArgObject(unsigned int index, void *value);
	void *GetAddressofArg(unsigned int index);

	asBYTE GetReturnByte(void);
	asWORD GetReturnWord(void);
	asDWORD GetReturnDWord(void);
	asQWORD GetReturnQWord(void);
	float GetReturnFloat(void);
	double GetReturnDouble(void);
	void *GetReturnAddress(void);
	void *GetReturnObject(void);

	bool Call(void *returnValue, int argumentCount, va_list arguments);
	bool Call(int argumentCount, va_list arguments);
	bool Call(void *returnValue, int argumentCount, ...);
	bool Call(int argumentCount, ...);
	bool Call(CASArguments &arguments, CASReturnValue *returnValue);
	CASReturnValue *Call(CASArguments &arguments, bool unused);

protected:
	virtual bool PreExecute(void);
	virtual void PostExecute(void);

	asIScriptContext *AcquireContext(void);
	void ReleaseContext(asIScriptContext *previousContext);
	bool CallInternal(bool unprepare, int argumentCount, va_list arguments);
	bool CallInternal(bool unprepare, CASArguments &arguments);

	asIScriptFunction	*m_pFunction;
	CASModule			*m_pModule;
	asIScriptContext	*m_pContext;

	static unsigned int m_uiInCallCount;
};

#endif
