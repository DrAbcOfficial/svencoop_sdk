#include "extdll.h"
#include "server/server_support.h"
#include "scripting/core/CASDirectory.h"

CASDirectory::CASDirectory(const char *name, unsigned char moduleTypes, unsigned char accessFlags,
	unsigned char directoryFlags, unsigned char tempFlags)
	: m_pParent(NULL), m_pFirstChild(NULL), m_pNextSibling(NULL),
	m_uiModuleTypes(moduleTypes), m_uiAccessFlags(accessFlags),
	m_uiDirectoryFlags(directoryFlags), m_uiTempFlags(tempFlags)
{
	UTIL_CopyString(m_szName, name, sizeof(m_szName));
}

CASDirectory::~CASDirectory()
{
}

bool CASDirectory::HasChild(const CASDirectory *child) const
{
	return child && child->m_pParent == this;
}

CASDirectory *CASDirectory::FindChild(const char *name)
{
	if (!name || !*name)
		return NULL;

	for (CASDirectory *pChild = m_pFirstChild; pChild; pChild = pChild->m_pNextSibling)
	{
		if (!strcmp(name, pChild->m_szName))
			return pChild;
	}

	return NULL;
}

const CASDirectory *CASDirectory::FindChild(const char *name) const
{
	return const_cast<CASDirectory *>(this)->FindChild(name);
}

bool CASDirectory::AddChild(CASDirectory *child)
{
	if (!child)
		return false;

	child->m_pNextSibling = m_pFirstChild;
	m_pFirstChild = child;
	child->m_pParent = this;
	return true;
}

void CASDirectory::RemoveChild(CASDirectory *child)
{
	if (!child || child->m_pParent != this)
		return;

	CASDirectory *pPrevious = NULL;
	CASDirectory *pChild = m_pFirstChild;

	while (pChild)
	{
		if (pChild == child)
		{
			if (pPrevious)
				pPrevious->m_pNextSibling = child->m_pNextSibling;
			else
				m_pFirstChild = child->m_pNextSibling;

			break;
		}

		pPrevious = pChild;
		pChild = pChild->m_pNextSibling;
	}

	child->m_pParent = NULL;
}
