#include "CASReturnValue.h"

CASReturnValue::CASReturnValue()
	: m_uiFlags(0)
{
}

CASReturnValue::~CASReturnValue()
{
}

void CASReturnValue::Release(void) const
{
	if (InternalRelease())
		delete this;
}

bool CASReturnValue::HasReturnValue(void) const
{
	return (m_ReturnValue.m_ArgumentType & (ARGUMENT_NONE | ARGUMENT_VOID)) == 0;
}
