#ifndef CASMAP_H
#define CASMAP_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CASMap : public CASServerScriptGlobal<CASMap>
{
public:
	virtual ~CASMap();

	bool HasForcedPlayerModels( void ) const;
	bool LoadMapSkillFile( void );
	bool LoadGlobalSkillFile( void );
	bool LoadSkillFile( const CString &fileName );
};

#endif
