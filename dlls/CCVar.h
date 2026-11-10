#ifndef CCVAR_H
#define CCVAR_H

#include "CBaseConCommand.h"
#include "CCVarArguments.h"
#include "CString.h"

class CASFile;
class CASVirtualFileSystem;

class CCVar : public CBaseConCommand
{
	friend void GetCVarValues(void *object, const char *&defaultValue, const char *&currentValue);

public:
	CCVar(const char *name, const CCVarArguments &arguments);
	virtual ~CCVar();
	virtual void Visit(CConCommandSystem &system, void *context);

	void SetInterfaceCallback(ICVarCallback *callback);
	void Dispatch(const CString &oldValue, float oldFloat);
	void RevertToDefault(void);
	void SetString(const char *value);
	void SetFloat(float value);
	void SetInt(int value);
	void SetBool(bool value);

private:
	friend class CConCommandSystem;
	friend CASFile *CASVirtualFileSystem_OpenFile(CASVirtualFileSystem *fileSystem,
		const CString &fileName, unsigned char mode);
	friend void log_commands_Callback(CCVar &variable, const CString &oldValue, float oldFloat);

	void SetStringInternal(const char *value, bool notify);

	const char *m_pszDefaultValue;
	CString	m_Value;
	float	m_flValue;
	union
	{
		CVarCallback_t	m_pFunctionCallback;
		ICVarCallback *m_pInterfaceCallback;
	};
	bool m_bInterfaceCallback;
};

#endif
