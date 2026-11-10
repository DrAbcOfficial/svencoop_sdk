#ifndef CASGAME_H
#define CASGAME_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CBaseEntity;
class asIScriptGeneric;

class CASGame : public CASServerScriptGlobal<CASGame>
{
public:
	virtual ~CASGame();

	bool AlertMessage( asIScriptGeneric *generic );
	int PrecacheModel( const CString &fileName );
	int PrecacheModel( CBaseEntity *entity, const CString &fileName );
	void PrecacheOther( const CString &className );
	void PrecacheMonster( const CString &className, bool ally );
	void PrecacheGeneric( const CString &fileName );

	CString GetGameName( void ) const;
	CString GetGameVersionString( void ) const;
	unsigned int GetGameVersion( void ) const;
};

#endif
