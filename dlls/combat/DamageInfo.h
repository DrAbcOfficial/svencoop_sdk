#ifndef DAMAGEINFO_H
#define DAMAGEINFO_H

class CBaseEntity;

struct DamageInfo
{
	CBaseEntity	*pVictim;
	CBaseEntity	*pInflictor;
	CBaseEntity	*pAttacker;
	float		flDamage;
	int			bitsDamageType;
};

void SetDamageInfo(DamageInfo *info, CBaseEntity *victim, CBaseEntity *inflictor,
	CBaseEntity *attacker, float damage, int damageType);
void GetDamageInfo(DamageInfo *info, CBaseEntity **victim, CBaseEntity **inflictor,
	CBaseEntity **attacker, float *damage, int *damageType);

#endif
