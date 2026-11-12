#ifndef TRIGGER_NUMERICDISPLAY_H
#define TRIGGER_NUMERICDISPLAY_H

class CTriggerNumericDisplay : public CBaseEntity
{
public:
	~CTriggerNumericDisplay() override {}

	void Spawn( void ) override;
	void Activate( void ) override;
	void OnDestroy( void ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

private:
	struct EntityList_t
	{
		EHANDLE			 entity;
		EntityList_t	*next;
	};

	struct Digit_t
	{
		Digit_t			*next;
		EntityList_t	*entities;
		unsigned int	 position;
	};

	void SetState( bool enabled );
	void Clear( void );
	int GetUpdatedValue( void );
	void RemoveDigit( Digit_t *digit, Digit_t *previous );
	void RemoveEntityFromDigit( Digit_t *digit, EntityList_t *entity, EntityList_t *previous );
	void Update( int value );
	void EXPORT UpdateThink( void );
	void FindDigits( void );

	Digit_t	*m_pDigits;
	bool	 m_bEnabled;
};

#endif
