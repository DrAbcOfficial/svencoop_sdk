#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "ai/basemonster.h"
#include "weapons/weapons.h"
#include "items/items.h"
#include "gamerules/game.h"
#include "server/CCustomEntity.h"
#include "server/CCustomKeyvalues.h"
#include "scripting/core/CASCustomEntityFuncs.h"
#include "scripting/core/CASItemRegistry.h"
#include "scripting/core/CASServerManager.h"
#include "scripting/core/CASDocumentation.h"
#include "scripting/core/CASModule.h"
#include "scripting/core/CASObjectType.h"
#include "scripting/core/CASObjectInstance.h"
#include "scripting/core/CASHook.h"
#include "scripting/core/CASLog.h"
#include "interface.h"
#include <angelscript.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

void EntvarsKeyvalue( entvars_t *pev, KeyValueData *pkvd );

template <class T>
CBaseEntity *CreateCustomEntityOfType( entvars_t *pev )
{
	return GetClassPtr( (T *) pev );
}

CBaseEntity *Create_CCustomBaseEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomBaseEntity<CBaseEntity> >( pev );
}

CBaseEntity *Create_CCustomBaseAnimating( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomBaseAnimating<CBaseAnimating> >( pev );
}

CBaseEntity *Create_CCustomMonsterEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomMonsterEntity<CBaseMonster> >( pev );
}

CBaseEntity *Create_CCustomPlayerItemEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomPlayerItemEntity<CBasePlayerItem> >( pev );
}

CBaseEntity *Create_CCustomPlayerWeaponEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomPlayerWeaponEntity<CBasePlayerWeapon> >( pev );
}

CBaseEntity *Create_CCustomItemEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomItemEntity<CItem> >( pev );
}

CBaseEntity *Create_CCustomPlayerAmmoEntity( entvars_t *pev )
{
	return CreateCustomEntityOfType<CBaseCustomPlayerAmmoEntity<CBasePlayerAmmo> >( pev );
}

static const CustomEntityData_t g_CustomEntityData[] =
{
	{ "ScriptBaseEntity", Create_CCustomBaseEntity },
	{ "ScriptBaseAnimating", Create_CCustomBaseAnimating },
	{ "ScriptBaseMonsterEntity", Create_CCustomMonsterEntity },
	{ "ScriptBaseTankEntity", Create_CCustomTankEntity },
	{ "ScriptBasePlayerItemEntity", Create_CCustomPlayerItemEntity },
	{ "ScriptBasePlayerWeaponEntity", Create_CCustomPlayerWeaponEntity },
	{ "ScriptBaseItemEntity", Create_CCustomItemEntity },
	{ "ScriptBasePlayerAmmoEntity", Create_CCustomPlayerAmmoEntity }
};

void *Sys_GetCurrentModule( void )
{
#ifdef _WIN32
	HMODULE module = NULL;

	GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
	                    (LPCSTR) &Sys_GetCurrentModule, &module );
	return module;
#else
	Dl_info info;

	if ( !dladdr( (void *) &Sys_GetCurrentModule, &info ) )
		return NULL;

	void *module = dlopen( info.dli_fname, RTLD_NOW | RTLD_NOLOAD );

	// dlopen added a reference to a library that is already loaded
	if ( module )
		dlclose( module );

	return module;
#endif
}

template <class BASE>
bool CBaseCustomPlayerItemEntity<BASE>::OnScriptClassReady( void )
{
	const CASItemRegistry::ItemData_t *pItem = CASItemRegistry::GetInstance()->GetItemInfoForName( STRING( this->pev->classname ) );

	if ( !pItem || !pItem->iId )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "ScriptBasePlayerItemEntity: item '%s' was not registered, removing item!\n",
		                              STRING( this->pev->classname ) );
		return false;
	}

	this->m_iId = pItem->iId;

	if ( !this->m_iszCustomSpriteDir )
		this->m_iszCustomSpriteDir = MAKE_STRING( pItem->szSpriteDir );

	return true;
}

template <class BASE>
bool CBaseCustomPlayerItemEntity<BASE>::GetItemInfo( ItemInfo *p )
{
	const CASItemRegistry::ItemData_t *pItem = CASItemRegistry::GetInstance()->GetItemInfoForName( STRING( this->pev->classname ) );

	if ( !pItem )
		return FALSE;

	p->pszName = pItem->szName;
	p->pszAmmo1 = pItem->szAmmo1[0] ? pItem->szAmmo1 : NULL;
	p->pszAmmo2 = pItem->szAmmo2[0] ? pItem->szAmmo2 : NULL;
	p->pszAmmo1Entity = pItem->szAmmo1Entity[0] ? pItem->szAmmo1Entity : NULL;
	p->pszAmmo2Entity = pItem->szAmmo2Entity[0] ? pItem->szAmmo2Entity : NULL;
	p->iId = pItem->iId;

	if ( !this->m_pHandler->MethodExists( "bool GetItemInfo(ItemInfo& out)" ) )
		return FALSE;

	bool fResult = false;

	this->m_pHandler->CallMethod( &fResult, false, "bool GetItemInfo(ItemInfo& out)", p );
	return fResult;
}

//=========================================================
// CASCustomEntityFuncs
//=========================================================
CASCustomEntityFuncs::CASCustomEntityFuncs()
{
}

CASCustomEntityFuncs::~CASCustomEntityFuncs()
{
}

void CASCustomEntityFuncs::Shutdown( void )
{
	Clear();
}

void CASCustomEntityFuncs::Clear( void )
{
	for ( EntityMap_t::iterator it = m_Entities.begin(); it != m_Entities.end(); it++ )
		it->second.pType->Release();

	m_Entities.clear();
}

bool CASCustomEntityFuncs::IsCustomEntity( const CString &entityName ) const
{
	if ( !entityName.Length() )
		return false;

	return m_Entities.find( entityName ) != m_Entities.end();
}

const CASCustomEntityFuncs::CustomEntityTypeData_t *CASCustomEntityFuncs::GetCustomEntityType( const CString &entityName ) const
{
	if ( !entityName.Length() )
		return NULL;

	EntityMap_t::const_iterator it = m_Entities.find( entityName );

	if ( it == m_Entities.end() )
		return NULL;

	return &it->second;
}

bool CASCustomEntityFuncs::IsDefaultEntity( const CString &entityName ) const
{
	if ( !entityName.Length() )
		return false;

	// every LINK_ENTITY_TO_CLASS is an export of this library
	return Sys_GetProcAddress( Sys_GetCurrentModule(), entityName.Data() ) != NULL;
}

bool CASCustomEntityFuncs::InheritsFromCustomEntity( CASObjectType *type, const CustomEntityData_t **data ) const
{
	*data = NULL;

	if ( !type )
		return false;

	for ( unsigned int i = 0; i < ARRAYSIZE( g_CustomEntityData ); i++ )
	{
		if ( type->DerivesFrom( g_CustomEntityData[i].pszBaseClass ) )
		{
			*data = &g_CustomEntityData[i];
			return true;
		}
	}

	return false;
}

void CASCustomEntityFuncs::RegisterCustomEntity( const CString &className, const CString &entityName )
{
	CString szClassName( className );
	CString szEntityName( entityName );

	szClassName.Trim();
	szEntityName.Trim();

	if ( !szClassName.Length() || !szEntityName.Length() )
		return;

	CString szNamespace;
	unsigned int iSeparator = szClassName.RFind( ':' );

	if ( iSeparator != CString::INVALID_INDEX )
	{
		// "a::b::Class" is class "Class" in namespace "a::b"
		szNamespace = szClassName.SubString( 0, iSeparator - 1 );
		szClassName = szClassName.SubString( iSeparator + 1 );
	}

	if ( IsCustomEntity( szEntityName ) )
	{
		CASLog::GetInstance()->Log( AS_LOG_DEFAULT, "CCustomEntityFuncs::RegisterCustomEntity: Cannot register entity '%s'; a custom entity by that name already exists!\n",
		                            szEntityName.Data() );
		return;
	}

	if ( IsDefaultEntity( szEntityName ) )
	{
		CASLog::GetInstance()->Log( AS_LOG_DEFAULT, "CCustomEntityFuncs::RegisterCustomEntity: Cannot register entity '%s'; a default entity by that name already exists!\n",
		                            szEntityName.Data() );
		return;
	}

	CASModule *pModule = CASServerManager::GetInstance()->GetMapModule();

	pModule->m_bHasCustomEntities = true;

	asIScriptModule *pScriptModule = pModule->m_pModule;
	CString szOldNamespace( pScriptModule->GetDefaultNamespace() );

	if ( pScriptModule->SetDefaultNamespace( szNamespace.Data() ) < 0 )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CCustomEntityFuncs::RegisterCustomEntity: Invalid namespace for class '%s%s'!\n",
		                              szNamespace.Data(), szClassName.Data() );
		return;
	}

	CASObjectType *pType = pModule->GetObjectType( szClassName.Data() );

	pScriptModule->SetDefaultNamespace( szOldNamespace.Data() );

	if ( !pType )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CCustomEntityFuncs::RegisterCustomEntity: No class called \"%s\"!\n", szClassName.Data() );
		return;
	}

	const CustomEntityData_t *pData;

	if ( !InheritsFromCustomEntity( pType, &pData ) )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "CCustomEntityFuncs::RegisterCustomEntity: Class \"%s\" is not an entity class!\n", szClassName.Data() );
		pType->Release();
		return;
	}

	CustomEntityTypeData_t typeData;

	typeData.pData = pData;
	typeData.pType = pType;
	m_Entities.insert( EntityMap_t::value_type( szEntityName, typeData ) );

	CASLog::GetInstance()->Log( AS_LOG_DEFAULT, "Registering class \"%s\" as custom entity \"%s\"\n", szClassName.Data(), szEntityName.Data() );
}

void CASCustomEntityFuncs::UnRegisterCustomEntity( const CString &entityName )
{
	CString szEntityName( entityName );

	szEntityName.Trim();

	if ( !szEntityName.Length() )
		return;

	EntityMap_t::iterator it = m_Entities.find( szEntityName );

	if ( it == m_Entities.end() )
		return;

	m_Entities.erase( it );
	CASLog::GetInstance()->Log( AS_LOG_DEFAULT, "Unregistering custom entity \"%s\"\n", szEntityName.Data() );
}

void RegisterSCScriptCustomEntityDependencies( CASDocumentation *documentation )
{
	documentation->RegisterFuncDef( "Function definition for custom entity Think functions",
	                                "void ThinkFunction()" );
	documentation->RegisterFuncDef( "Function definition for custom entity Use functions",
	                                "void UseFunction(CBaseEntity@ pActivator, CBaseEntity@ pCaller, USE_TYPE useType, float flValue)" );
	documentation->RegisterFuncDef( "Function definition for custom entity Touch functions",
	                                "void TouchFunction(CBaseEntity@ pOther)" );
	documentation->RegisterFuncDef( "Function definition for custom entity Blocked functions",
	                                "void BlockedFunction(CBaseEntity@ pOther)" );

	RegisterSCScriptCustomEntityCallbackHandler( documentation );

	asDWORD accessMask = documentation->SetDefaultAccessMask( AS_ACCESS_ALL );

	documentation->RegisterObjectType( "Custom entity functions.", "CCustomEntityFuncs", 0, asOBJ_REF | asOBJ_NOCOUNT );
	documentation->RegisterObjectMethod(
	    "Returns whether the given entity name is a registered custom entity",
	    "CCustomEntityFuncs",
	    "bool IsCustomEntity(const string& in szEntityname) const",
	    asMETHOD( CASCustomEntityFuncs, IsCustomEntity ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Registers a class called szClassname as a custom entity named szEntityname. The class must inherit from a script entity base class.",
	    "CCustomEntityFuncs",
	    "void RegisterCustomEntity(const string& in szClassname, const string& in szEntityname)",
	    asMETHOD( CASCustomEntityFuncs, RegisterCustomEntity ),
	    asCALL_THISCALL );
	documentation->RegisterObjectMethod(
	    "Unregisters a custom entity by the given name",
	    "CCustomEntityFuncs",
	    "void UnRegisterCustomEntity(const string& in szEntityname)",
	    asMETHOD( CASCustomEntityFuncs, UnRegisterCustomEntity ),
	    asCALL_THISCALL );
	documentation->RegisterGlobalProperty( "Custom entity functions global instance.", "CCustomEntityFuncs g_CustomEntityFuncs",
	                                       CASCustomEntityFuncs::GetInstance() );

	documentation->SetDefaultAccessMask( accessMask );
}

//=========================================================
// Creation
//=========================================================
CBaseEntity *CreateCustomEntity( const char *pszClassName, entvars_t *pev )
{
	const CASCustomEntityFuncs::CustomEntityTypeData_t *pTypeData = CASCustomEntityFuncs::GetInstance()->GetCustomEntityType( CString( pszClassName ) );

	if ( !pTypeData )
	{
		if ( sv_report_unknown_entities.value != 0 )
			CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Cannot instantiate entity: Entity '%s' does not exist!\n", pszClassName );

		if ( !pev->pContainingEntity->free )
			REMOVE_ENTITY( pev->pContainingEntity );

		return NULL;
	}

	CBaseEntity *pEntity = pTypeData->pData->pfnFactory( pev );

	if ( !pEntity )
	{
		if ( !pev->pContainingEntity->free )
			REMOVE_ENTITY( pev->pContainingEntity );

		return NULL;
	}

	if ( !pev->classname )
	{
		KeyValueData kvd;

		kvd.szClassName = pszClassName;
		kvd.szKeyName = "classname";
		kvd.szValue = pszClassName;
		kvd.fHandled = FALSE;
		EntvarsKeyvalue( pev, &kvd );
	}

	ICustomEntity *pCustom = pEntity->MyCustomPointer();
	bool fFailed = true;

	if ( !pCustom )
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Custom entity: target class does not implement ICustomEntity '%s'!\n", pszClassName );
	}
	else
	{
		CASObjectInstance *pInstance = pTypeData->pType->CreateInstance();

		if ( !pInstance || !pInstance->m_pObject )
		{
			CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Custom entity: Could not instantiate class '%s', did you create a non-default constructor?\n",
			                              pszClassName );
		}
		else
		{
			pCustom->SetScriptInstance( pInstance );
			pInstance->Release();

			if ( !pInstance->CallMethod( false, "void SetSelf(CBaseEntity@)", 0, pEntity )
			     || !pInstance->CallMethod( false, "void SetPev(entvars_t@)", 0, pEntity->pev )
			     || !pInstance->CallMethod( false, "void SetBaseClass(BaseEntity@)", 0, pEntity )
			     || !pInstance->CallMethod( false, "void SetCallbackHandler(CustomEntityCallbackHandler@)", 0, pCustom->GetHandler() ) )
			{
				CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Custom entity: Could not initalize class '%s', did you override script entity binding methods?\n",
				                              pszClassName );
			}
			else
			{
				pEntity->SetPlayerAlly( false );
				pInstance->CallMethod( true, "void OnCreate()", 0 );

				pEntity = CBaseEntity::Instance( pev->pContainingEntity );

				if ( pEntity && !( pev->flags & FL_KILLME ) )
					return pEntity;

				CASLog::GetInstance()->Warning( AS_LOG_DEFAULT, "Custom entity: Do not remove an entity in its OnCreate method!\n" );

				if ( !pEntity )
				{
					if ( !pev->pContainingEntity->free )
						REMOVE_ENTITY( pev->pContainingEntity );

					return NULL;
				}
			}
		}
	}

	REMOVE_ENTITY( ENT( pEntity->pev ) );
	return NULL;
}

edict_t *ASCreateCustomEntity( const char *pszClassName )
{
	edict_t *pent = CREATE_ENTITY();

	if ( !pent )
	{
		CASLog::GetInstance()->Warning( AS_LOG_DEFAULT, "ASCreateCustomEntity: out of edicts!\n" );
		return NULL;
	}

	CBaseEntity *pEntity = CreateCustomEntity( pszClassName, VARS( pent ) );

	if ( !pEntity )
		return NULL;

	if ( CASServerManager::GetInstance()->IsScriptLifecycleEntered() )
	{
		DispatchEntityCreatedHook( pEntity );
	}
	else
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "ASCreateCustomEntity: Called before map script was compiled!\n" );
		REMOVE_ENTITY( pent );
	}

	return pEntity->edict();
}

edict_t *ASCreateCustomEntity( string_t iszClassName )
{
	return ASCreateCustomEntity( STRING( iszClassName ) );
}

//=========================================================
// The engine spawns an entity it has no class for as
// "custom" with its classname in "customclass"
//=========================================================
class CCustomEntityAdapter : public CBaseEntity
{
public:
	void OnCreate( void );
	CCustomEntityAdapter();
	void KeyValue( KeyValueData *pkvd );
};

static CCustomEntityAdapter g_CustomEntityAdapter;

void CCustomEntityAdapter::OnCreate( void )
{
}

CCustomEntityAdapter::CCustomEntityAdapter()
{
	pev = NULL;
	delete m_pCustomKeyvalues;
	m_pCustomKeyvalues = NULL;
}

extern "C" DLLEXPORT void custom( entvars_t *pev )
{
	CASServerManager *manager = CASServerManager::GetInstance();

	if ( !manager->m_bScriptLifecycleEntered )
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Custom entity created before map script was compiled!\n" );

	if ( manager->m_bScriptLifecycleEntered && manager->m_bBuildSucceeded )
	{
		pev->pContainingEntity->pvPrivateData = &g_CustomEntityAdapter;
		g_CustomEntityAdapter.pev = pev;
	}
	else
		pev->flags |= FL_KILLME;
}

void CCustomEntityAdapter::KeyValue( KeyValueData *pkvd )
{
	if ( !strcmp( pkvd->szKeyName, "customclass" ) )
	{
		// let the factory allocate the real entity in this one's place
		pev->pContainingEntity->pvPrivateData = NULL;

		if ( !CreateCustomEntity( pkvd->szValue, pev ) )
			pev->flags |= FL_KILLME;

		pev = NULL;
	}
	else
	{
		CASLog::GetInstance()->Error( AS_LOG_DEFAULT, "Custom entity keyvalue was not customclass!\n" );
		pev->flags |= FL_KILLME;
		pev->pContainingEntity->pvPrivateData = NULL;
	}
}
