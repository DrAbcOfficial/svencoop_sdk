#ifndef GARGANTUA_H
#define GARGANTUA_H

class CStomp : public CBaseEntity
{
public:
	void           Spawn( void );
	void           Think( void );
	static CStomp *StompCreate( const Vector &origin, const Vector &end, float speed, entvars_t *pevOwner );
};

void StreakSplash( const Vector &origin, const Vector &direction, int color, int count, int speed, int velocityRange );

#endif
