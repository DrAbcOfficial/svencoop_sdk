#include "CASBaseClass.h"

#include <angelscript.h>

CASBaseClass::CASBaseClass()
{
}

CASBaseClass::~CASBaseClass()
{
}

void CASRefCountedBaseClass::AddRef(void) const
{
	m_iRefCount++;
}

int CASRefCountedBaseClass::GetRefCount(void) const
{
	return m_iRefCount;
}

bool CASRefCountedBaseClass::InternalRelease(void) const
{
	return m_iRefCount-- == 1;
}

void CASAtomicRefCountedBaseClass::AddRef(void) const
{
	asAtomicInc(m_iRefCount);
}

bool CASAtomicRefCountedBaseClass::InternalRelease(void) const
{
	return asAtomicDec(m_iRefCount) == 0;
}
