#ifndef CASWAYPOINTMANAGER_H
#define CASWAYPOINTMANAGER_H

#include "CASServerScriptGlobal.h"
#include "CString.h"

class CASDocumentation;
class CASFunction;

struct PathConditionPair_t
{
	PathConditionPair_t *m_pNext;
	char m_szName[132];
	CASFunction *m_pStartFunction;
	CASFunction *m_pStopFunction;
};

class CASWaypointManager : public CASServerScriptGlobal<CASWaypointManager>
{
public:
	virtual ~CASWaypointManager();

	static bool IsValidName( const char *name );

	PathConditionPair_t *Find( const char *name, PathConditionPair_t **found ) const;
	PathConditionPair_t *GetConditions( const char *name ) const;

	void InsertPair( PathConditionPair_t *pair );

	bool RegisterConditions( const CString &name, asIScriptFunction *startFunction, asIScriptFunction *stopFunction );

	void FreePair( PathConditionPair_t *pair );

	void RemovePair( PathConditionPair_t *pair, PathConditionPair_t *previous );

	void Clear( void );

	virtual void Shutdown( void );

private:
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASWaypointManager>;

	CASWaypointManager();

	PathConditionPair_t *m_pFirst;
};

void RegisterSCScriptWaypointManager( CASDocumentation *documentation );

#endif
