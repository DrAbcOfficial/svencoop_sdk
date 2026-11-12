#ifndef CASFUNCTION_H
#define CASFUNCTION_H

#include "scripting/core/CASBaseCallable.h"

class CASFunction : public CASBaseCallable
{
public:
	CASFunction(asIScriptFunction *function, CASModule *module);
	virtual ~CASFunction();

	static CASFunction *Create(asIScriptFunction *function, CASModule *module, bool releaseFunction);
};

#endif
