#include "CASFunction.h"

CASFunction::CASFunction(asIScriptFunction *function, CASModule *module)
	: CASBaseCallable(function, module)
{
}

CASFunction::~CASFunction()
{
}

CASFunction *CASFunction::Create(asIScriptFunction *function, CASModule *module, bool releaseFunction)
{
	if (!function)
		return NULL;

	CASFunction *result = new CASFunction(function, module);

	if (releaseFunction)
		function->Release();

	return result;
}
