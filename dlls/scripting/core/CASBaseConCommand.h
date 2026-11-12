#ifndef CASBASECONCOMMAND_H
#define CASBASECONCOMMAND_H

#include "scripting/core/CASGCBaseClass.h"
#include "server/ConCommandDefs.h"
#include "CString.h"

class CASConCommandSystem;
class CASFunction;
class asIScriptFunction;
class asIScriptEngine;

class CASBaseConCommand : public CASGCBaseClass<CASRefCountedBaseClass>
{
public:
	CASBaseConCommand(ConCommandKind::Type kind, const CString &name, ConCommandFlags_t flags,
	                  const CString &helpInfo, asIScriptFunction *callback);

	virtual CString GetAdditionalInfo(void) const;
	virtual void Visit(CASConCommandSystem &system, void *context) = 0;
	virtual ~CASBaseConCommand();

	void Release(void) const;
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseReferences(asIScriptEngine *engine);
	void SetCallback(CASFunction *callback);
	void SetCallback(asIScriptFunction *callback);
	ConCommandKind::Type GetKind(void) const;
	const CString &GetName(void) const;
	const CString &GetFullyQualifiedName(void) const;
	const CString &GetHelpInfo(void) const;
	const CString &GetOwningModuleName(void) const;
	bool HasBeenAdded(void) const;

protected:
	friend class CASConCommandSystem;

	ConCommandKind::Type	m_Kind;
	ConCommandFlags_t		m_Flags;
	CString					m_Name;
	CString					m_FullyQualifiedName;
	CString					m_HelpInfo;
	CASFunction *m_pCallback;
	CString					m_OwningModuleName;
	bool					m_bAdded;
};

#endif
