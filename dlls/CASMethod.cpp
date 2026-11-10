#include "CASMethod.h"

CASMethod::CASMethod(asIScriptFunction *function, CASModule *module)
	: CASBaseCallable(function, module), m_pThisPointer(NULL)
{
}

CASMethod::~CASMethod()
{
}

void *CASMethod::GetThisPointer(void) const
{
	return m_pThisPointer;
}

void CASMethod::SetThisPointer(void *object)
{
	m_pThisPointer = object;
}

bool CASMethod::PreExecute(void)
{
	return m_pContext->SetObject(m_pThisPointer) >= 0;
}

void CASMethod::PostExecute(void)
{
	m_pThisPointer = NULL;
}
