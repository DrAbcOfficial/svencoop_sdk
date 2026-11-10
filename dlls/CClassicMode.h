#ifndef CCLASSICMODE_H
#define CCLASSICMODE_H

#include "CASBaseClass.h"
#include "CASServerScriptGlobal.h"
#include "CString.h"

class CScriptArray;

class CClassicMode : public CASServerScriptGlobal<CClassicMode>
{
public:
	class CItemMapping : public CASRefCountedBaseClass
	{
	public:
		CItemMapping( const CString &from, const CString &to );

		const CString &GetFrom( void ) const;
		const CString &GetTo( void ) const;
		void Release( void ) const;

	private:
		CString m_From;
		CString m_To;
	};

	CClassicMode();
	virtual ~CClassicMode();

	virtual void Initialize( void );
	virtual void Shutdown( void );
	virtual void Think( void );
	virtual void PreMapInit( void );
	virtual void PostMapInit( bool success );
	virtual void PostMapActivate( bool success );

	bool IsStateDefined( void ) const;
	void ResetState( void );
	bool IsEnabled( void ) const;
	void ForceItemRemap( bool enabled );
	void SetShouldRestartOnChange( bool restart );
	bool ShouldRestartOnChange( void ) const;
	void Toggle( void );
	void SetEnabled( bool enabled );
	void SetItemMappings( CScriptArray *mappings );
	void EnableMapSupport( void );
	CItemMapping *ItemMappingAt( unsigned int index ) const;
	const CString &FindMappedItem( const char *name ) const;
	const CString &FindMappedItem( const CString &name ) const;

private:
	friend class CVoteManager;
	class CPlayerItemInfo;
	void UpdateWorld( bool includePlayers );

	bool          m_bForceItemRemap;
	CScriptArray *m_pItemMappings;

	bool m_bMapSupport;
	bool m_bShouldRestartOnChange;

	int m_iState;
	int m_iNextState;
};

CClassicMode::CItemMapping *ConstructItemMapping( const CString &from, const CString &to );

class CClassicModeWrapper
{
public:
	static bool isEnabled( void ) { return CClassicMode::GetInstance()->IsEnabled(); }
};

#endif
