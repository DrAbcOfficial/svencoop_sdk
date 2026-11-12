#ifndef CASSERVERREGISTRATION_H
#define CASSERVERREGISTRATION_H

#include "scripting/core/CASBaseRegistration.h"

class CASServerRegistration : public CASBaseRegistration
{
public:
	CASServerRegistration();
	virtual ~CASServerRegistration();

	virtual void RegisterToStringCallbacks(asIScriptEngine *engine);
	virtual void RegisterInterfaces(CASDocumentation *documentation);
};

#endif
