#ifndef CASSERVERSCRIPTGLOBAL_H
#define CASSERVERSCRIPTGLOBAL_H

#include "scripting/core/CASBaseScriptGlobal.h"
#include "scripting/core/IASServerScriptGlobal.h"

template<class T>
class CASServerScriptGlobal : public CASBaseScriptGlobal<IASServerScriptGlobal, T>
{
public:
	CASServerScriptGlobal() {}
	virtual ~CASServerScriptGlobal() {}

	virtual void PreMapInit(void) {}
	virtual void PostMapInit(bool success) {}
	virtual void PreMapActivate(void) {}
	virtual void PostMapActivate(bool success) {}
};

#endif
