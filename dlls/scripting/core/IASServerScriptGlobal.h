#ifndef IASSERVERSCRIPTGLOBAL_H
#define IASSERVERSCRIPTGLOBAL_H

#include "scripting/core/IASScriptGlobal.h"

class IASServerScriptGlobal : public IASScriptGlobal
{
public:
	virtual void PreMapInit(void) = 0;
	virtual void PostMapInit(bool success) = 0;
	virtual void PreMapActivate(void) = 0;
	virtual void PostMapActivate(bool success) = 0;
};

#endif
