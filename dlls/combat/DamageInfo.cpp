#include "combat/DamageInfo.h"

void SetDamageInfo(DamageInfo *info, CBaseEntity *victim, CBaseEntity *inflictor,
	CBaseEntity *attacker, float damage, int damageType)
{
	info->pVictim = victim;
	info->pInflictor = inflictor;
	info->pAttacker = attacker;
	info->flDamage = damage;
	info->bitsDamageType = damageType;
}

void GetDamageInfo(DamageInfo *info, CBaseEntity **victim, CBaseEntity **inflictor,
	CBaseEntity **attacker, float *damage, int *damageType)
{
	if (victim)
		*victim = info->pVictim;

	if (inflictor)
		*inflictor = info->pInflictor;

	if (attacker)
		*attacker = info->pAttacker;

	if (damage)
		*damage = info->flDamage;

	if (damageType)
		*damageType = info->bitsDamageType;
}
