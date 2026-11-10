#ifndef CASFUNCTION_H
#define CASFUNCTION_H

#include "CASBaseCallable.h"

class CASFunction : public CASBaseCallable
{
public:
	CASFunction(asIScriptFunction *function, CASModule *module);
	virtual ~CASFunction();

	static CASFunction *Create(asIScriptFunction *function, CASModule *module, bool releaseFunction);
};

#endif
