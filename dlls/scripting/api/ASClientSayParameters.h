#ifndef ASCLIENTSAYPARAMETERS_H
#define ASCLIENTSAYPARAMETERS_H

#include "scripting/core/CASBaseClass.h"
#include "CString.h"

class CBasePlayer;
class CASCommand;
class CASDocumentation;

class CASSayParameters : public CASRefCountedBaseClass
{
public:
	CASSayParameters(CBasePlayer *player, int sayType, const char *command);
	~CASSayParameters();

	void Release(void) const;
	CBasePlayer *GetPlayer(void) const;
	int GetSayType(void) const;
	const CString &GetCommand(void) const;
	const CASCommand *GetArguments(void) const;
	bool ShouldHide(void) const;
	void SetShouldHide(bool shouldHide);

private:
	CBasePlayer	*m_pPlayer;
	int			m_iSayType;
	CString		m_szCommand;
	CASCommand	*m_pArguments;
	bool		m_bShouldHide;
};

void RegisterSCScriptClientSayParameters(CASDocumentation *documentation);

#endif
