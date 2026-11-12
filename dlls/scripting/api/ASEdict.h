#ifndef ASEDICT_H
#define ASEDICT_H

class CASDocumentation;
class CString;
struct entvars_s;
struct edict_s;

bool EntityVars_ClassNameIs(const CString &className, entvars_s *variables);
unsigned char EntityVars_GetController(unsigned int index, entvars_s *variables);
void EntityVars_SetController(unsigned int index, unsigned char value, entvars_s *variables);
unsigned char EntityVars_GetBlending(unsigned int index, entvars_s *variables);
void EntityVars_SetBlending(unsigned int index, unsigned char value, entvars_s *variables);
bool EntityVars_SpawnFlagBitSet(int flags, entvars_s *variables);
bool EntityVars_FlagBitSet(int flags, entvars_s *variables);
edict_s *EntityVars_GetContainingEntity(entvars_s *variables);
short Edict_GetLeafNums(unsigned int index, edict_s *edict);

void RegisterSCScriptEdict(CASDocumentation *documentation);

#endif
