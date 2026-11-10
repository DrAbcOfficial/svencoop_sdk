#include "CASModuleHandle.h"
#include "CASModuleBuilder.h"
#include "CASModule.h"

CASModuleHandle::CASModuleHandle(ModuleType::Type type, const char *name)
    : m_pBuilder(new CASModuleBuilder(type, name)), m_pModule(NULL), m_bPendingBuild(true)
{
}

CASModuleHandle::~CASModuleHandle()
{
	if (m_pModule)
	{
		m_pModule->Invalidate();
		m_pModule->Release();
		m_pModule = NULL;
	}

	FreeBuilder();
}

const char *CASModuleHandle::GetModuleName(void) const
{
	if (m_pBuilder)
		return m_pBuilder->GetModuleName();

	if (m_pModule)
		return m_pModule->GetModuleName();

	return "";
}

bool CASModuleHandle::Build(bool freeBuilder)
{
	if (!m_bPendingBuild)
		return true;

	bool success = m_pBuilder->Build(&m_pModule);

	if (freeBuilder)
		FreeBuilder();

	m_bPendingBuild = false;
	return success;
}

void CASModuleHandle::FreeBuilder(void)
{
	if (m_pBuilder)
	{
		delete m_pBuilder;
		m_pBuilder = NULL;
	}
}
