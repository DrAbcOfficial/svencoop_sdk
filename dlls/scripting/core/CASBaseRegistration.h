#ifndef CASBASEREGISTRATION_H
#define CASBASEREGISTRATION_H

#include "scripting/core/CASBaseClass.h"

class asIScriptEngine;
class CASDocumentation;

class CASBaseRegistration : public CASBaseClass
{
public:
	CASBaseRegistration();
	virtual ~CASBaseRegistration();

	void RegisterGameInterface(CASDocumentation *documentation);

	virtual void RegisterToStringCallbacks(asIScriptEngine *engine);
	virtual void RegisterInterfaces(CASDocumentation *documentation);
	virtual void RegisterDependentInterfaces(CASDocumentation *documentation);
};

#endif
