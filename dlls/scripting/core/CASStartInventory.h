#ifndef CASSTARTINVENTORY_H
#define CASSTARTINVENTORY_H

#include "scripting/core/CASServerScriptGlobal.h"
#include "CString.h"

class CASStartInventory : public CASServerScriptGlobal<CASStartInventory>
{
public:
	virtual ~CASStartInventory();

	bool IsAvailable( void ) const;
	bool Exists( const CString &entry ) const;
	void Set( const CString &entry, unsigned long amount );
	void Set( const CString &entry );
	void Add( const CString &entry, unsigned long amount, bool add = true );
	void Remove( const CString &entry );
	bool Restock( const CString &entry, unsigned long amount );
	bool Limit( const CString &entry, unsigned long amount );
	unsigned long GetAmount( const CString &entry ) const;
};

#endif
