#ifndef CASCVAR_H
#define CASCVAR_H

#include "scripting/core/CASBaseConCommand.h"

class CASCVar : public CASBaseConCommand
{
public:
	CASCVar(const CString &name, const CString &defaultValue, const CString &helpInfo,
	        ConCommandFlags_t flags, asIScriptFunction *callback);
	CASCVar(const CString &name, float defaultValue, const CString &helpInfo,
	        ConCommandFlags_t flags, asIScriptFunction *callback);
	virtual ~CASCVar();
	virtual CString GetAdditionalInfo(void) const;
	virtual void Visit(CASConCommandSystem &system, void *context);

	const CString &GetDefaultValue(void) const;
	const CString &GetString(void) const;
	float GetFloat(void) const;
	int GetInt(void) const;
	bool GetBool(void) const;
	void SetString(const CString &value);
	void SetFloat(float value);
	void SetInt(int value);
	void SetBool(bool value);
	void RevertToDefault(void);

private:
	friend class CASConCommandSystem;

	bool SetStringInternal(const CString &value, bool notify);

	CString	m_DefaultValue;
	float	m_flValue;
	CString	m_Value;
};

#endif
