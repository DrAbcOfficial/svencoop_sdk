#ifndef SERVER_ENTVARS_H
#define SERVER_ENTVARS_H

#include "entvars_types.h"

class CCustomKeyvalues;
struct entvars_s;

enum { INVALID_ENTVAR_INDEX = -1, ENTVAR_FIELD_COUNT = 123 };

extern CCustomKeyvalues *gEntVarList;

void RemoveGlobalEntvarsList(void);
entvartype_s GetEntvarField(entvars_s *vars, const char *name, void **value, int &index);
int ValueForPEVFlag( const char *name, int index, bool &valid );
const char *ChangeValueTypeStr( int type );

#endif
