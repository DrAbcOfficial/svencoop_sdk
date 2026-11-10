#ifndef SERVER_PORTAL_H
#define SERVER_PORTAL_H

#include "EntityHandle.h"
#include <unordered_map>
#include <unordered_set>

class CBasePlayer;
class CPortal;
struct msurface_s;
struct texture_s;

typedef std::unordered_set<EntityHandle<CPortal>, EntityHandle<CPortal>::Hash, EntityHandle<CPortal>::Equal> PortalSet;
extern PortalSet globalPortals;

#define SF_PORTAL_MOVABLE 1
#define SF_PORTAL_START_OFF 2
#define SF_PORTAL_TELEPORT 4
#define SF_PORTAL_NO_PLANES 8
#define SF_PORTAL_HALF_FRAME 16
#define SF_PORTAL_SHOOT_THROUGH 32

enum PortalMode
{
	PORTAL_MIRROR = 0,
	PORTAL_MONITOR,
	PORTAL_PORTAL
};

enum PortalTextureMode
{
	PORTAL_TEXTURE_TRANSPARENT = 0,
	PORTAL_TEXTURE_ALL,
	PORTAL_TEXTURE_NAMED
};

class CPortal : public CBaseEntity
{
public:
	CPortal();
	virtual ~CPortal();

	void Spawn( void );
	void KeyValue( KeyValueData *data );
	void Touch( CBaseEntity *other );
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value );
	void EXPORT PortalThink( void );

	bool IsActive( void ) const { return m_bActive; }
	int GetPortalMode( void ) const { return m_iPortalMode; }
	bool IsShootThroughEnabled( void ) const { return ( pev->spawnflags & SF_PORTAL_SHOOT_THROUGH ) != 0; }
	const Vector &GetOrigin( void ) const { return pev->origin; }
	const Vector &GetAngles( void ) const { return pev->angles; }
	const Vector &GetSourceOrigin( void );
	const Vector &GetSourceAngles( void );
	float GetZoom( void ) const { return m_flZoom; }
	EntityHandle<CBaseEntity> GetSource( void );

	typedef std::unordered_set<msurface_s *> SurfaceSet;
	const SurfaceSet &GetPortalSurfaces( void ) const { return m_PortalSurfaces; }

private:
	void InitModel( void );
	bool IsPortalTexture( texture_s *texture );
	void SendClientUpdate( void );
	bool CheckValuesHaveChanged( void );
	void BackupValues( void );

	string_t					m_iszModel;
	SurfaceSet					m_PortalSurfaces;
	bool						m_bActive;
	EntityHandle<CBaseEntity>	m_hSource;
	string_t					m_iszTarget;
	bool						m_bSourceChanged;
	int							m_iPortalMode;
	float						m_flZoom;
	int							m_iFPS;
	int							m_iMinDistance;
	int							m_iMaxDistance;
	int							m_iTextureMode;
	string_t					m_iszTextureName;
	unsigned int				m_uiWidth;
	unsigned int				m_uiHeight;

	typedef std::unordered_map<EntityHandle<CBasePlayer>, float, EntityHandle<CBasePlayer>::Hash, EntityHandle<CBasePlayer>::Equal> ClientUpdateMap;
	ClientUpdateMap m_ClientUpdates;

	bool						m_bOldNoPlanes;
	bool						m_bOldShootThrough;
	bool						m_bOldHalfFrame;
	bool						m_bOldActive;
	Vector						m_vecOldOrigin;
	Vector						m_vecOldAngles;
	int							m_iOldPortalMode;
	int							m_iOldTextureMode;
	float						m_flOldZoom;
	EntityHandle<CBaseEntity>	m_hOldSource;
	bool						m_bOldSourceInvalid;
	Vector						m_vecOldSourceOrigin;
	Vector						m_vecOldSourceAngles;
	unsigned int				m_uiOldWidth;
	unsigned int				m_uiOldHeight;
	int							m_iOldFPS;
	int							m_iOldMinDistance;
	int							m_iOldMaxDistance;
};

#endif
