#ifndef CASENTITYLOADER_H
#define CASENTITYLOADER_H

#include "scripting/core/CASServerScriptGlobal.h"
#include "CString.h"

class CASDocumentation;
class CKvBlockNode;
class Vector;
struct edict_s;

class CASEntityLoader : public CASServerScriptGlobal<CASEntityLoader>
{
public:
	virtual ~CASEntityLoader();

	bool LoadFromFile( const CString &fileName, const Vector &offset );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASEntityLoader>;

	CASEntityLoader();

	void HandleKeyValues( const CString &className, edict_s *entity, CKvBlockNode &block ) const;
	bool InstanceEntity( CKvBlockNode &block, const Vector &offset ) const;
};

void RegisterSCScriptEntityLoader( CASDocumentation *documentation );

#endif
