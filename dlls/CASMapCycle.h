#ifndef CASMAPCYCLE_H
#define CASMAPCYCLE_H

#include "CASServerScriptGlobal.h"
#include "CString.h"
#include <stddef.h>

class CScriptArray;
struct mapcycle_item_s;

class CASMapCycle : public CASServerScriptGlobal<CASMapCycle>
{
public:
	virtual ~CASMapCycle();

	size_t Count( void ) const;
	CString GetNextMap( void ) const;
	CString GetNextMap( const CString &mapName ) const;
	CScriptArray *GetMapCycle( void ) const;
	mapcycle_item_s *GetMapInfo( const CString &mapName ) const;

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASMapCycle>;

	CASMapCycle();
};

#endif
