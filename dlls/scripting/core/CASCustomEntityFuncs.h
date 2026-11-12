#ifndef CASCUSTOMENTITYFUNCS_H
#define CASCUSTOMENTITYFUNCS_H

#include "scripting/core/CASServerScriptGlobal.h"
#include "scripting/core/CASFileQuota.h"
#include "CString.h"
#include <map>

class CBaseEntity;
class CASDocumentation;
class CASObjectType;
typedef struct entvars_s entvars_t;

// A script entity base class and the C++ class that hosts it
struct CustomEntityData_t
{
	const char	*pszBaseClass;
	CBaseEntity	*( *pfnFactory )( entvars_t *pev );
};

class CASCustomEntityFuncs : public CASServerScriptGlobal<CASCustomEntityFuncs>
{
public:
	struct CustomEntityTypeData_t
	{
		const CustomEntityData_t	*pData;
		CASObjectType				*pType;
	};

	virtual ~CASCustomEntityFuncs();
	virtual void Shutdown( void );

	bool IsCustomEntity( const CString &entityName ) const;
	const CustomEntityTypeData_t *GetCustomEntityType( const CString &entityName ) const;
	void RegisterCustomEntity( const CString &className, const CString &entityName );
	void UnRegisterCustomEntity( const CString &entityName );
	void Clear( void );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASCustomEntityFuncs>;

	typedef std::map<CString, CustomEntityTypeData_t, CStringLess> EntityMap_t;

	CASCustomEntityFuncs();

	bool IsDefaultEntity( const CString &entityName ) const;
	bool InheritsFromCustomEntity( CASObjectType *type, const CustomEntityData_t **data ) const;

	EntityMap_t	m_Entities;
};

void RegisterSCScriptCustomEntityDependencies( CASDocumentation *documentation );

#endif
