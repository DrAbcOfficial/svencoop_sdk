#ifndef CUSTOM_PRECACHE_H
#define CUSTOM_PRECACHE_H

class CCustomPrecache : public CBaseEntity
{
public:
	CCustomPrecache();

	void Spawn( void ) override;
	void Precache( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	bool ParseKeyValue( const char *name, const char *value );

private:
	void AddModel( string_t model );
	void AddSound( string_t sound );
	void AddSky( string_t sky );
	void FreePointers( void );
	void AddResource( string_t *&resources, int &count, string_t value );

	string_t	*m_pModels;
	string_t	*m_pSounds;
	string_t	*m_pSkies;

	int		m_iModelCount;
	int		m_iSoundCount;
	int		m_iSkyCount;
};

#endif
