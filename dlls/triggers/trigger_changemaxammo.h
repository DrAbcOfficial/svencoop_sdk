#ifndef TRIGGER_CHANGEMAXAMMO_H
#define TRIGGER_CHANGEMAXAMMO_H

class CTriggerChangeMaxAmmo : public CBaseEntity
{
public:
	void KeyValue( KeyValueData *pkvd ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;

private:
	struct Set_s
	{
		unsigned int	index;
		int				value;

		void operator()( CBasePlayer *player ) const;
	};

	struct Reset_s
	{
		unsigned int	index;
		int				value;

		void operator()( CBasePlayer *player ) const;
	};

	struct Add_s
	{
		unsigned int	index;
		int				value;

		void operator()( CBasePlayer *player ) const;
	};

	struct Subtract_s
	{
		unsigned int	index;
		int				value;

		void operator()( CBasePlayer *player ) const;
	};

	template <class Operation>
	void ChangeAmmo( CBaseEntity *activator, const Operation &operation );

	template <class Operation>
	void ChangeAmmoForPlayer( CBasePlayer *player, const Operation &operation );

	int	m_iMaxAmmo;
	int	m_Mode;
};

#endif
