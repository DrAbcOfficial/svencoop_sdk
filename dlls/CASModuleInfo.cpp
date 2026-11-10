#include "CASModuleInfo.h"
#include "CASModuleBuilder.h"
#include "CASHeaderList.h"
#include "CASLog.h"
#include <angelscript.h>
#include <stdio.h>

static const unsigned int DATABASE_SHIFT_COUNT_MASK = 0x1F;
static const size_t VALIDATION_ERROR_BUFFER_LENGTH = 256;

CASModuleInfo *CASModuleInfo::m_pHead;

CASModuleInfo::CASModuleInfo(const char *directory, ModuleType::Type type, unsigned char databaseTypes,
	bool precacheScripts, const char *initDeclaration)
	: m_pNext(m_pHead), m_pszDirectory(directory), m_ModuleType(type), m_uiDatabaseTypes(databaseTypes),
	  m_bPrecacheScripts(precacheScripts), m_pszInitDeclaration(initDeclaration)
{
	m_pHead = this;
}

CASModuleInfo::~CASModuleInfo()
{
	// Descriptors remain registered for the process lifetime
}

CASHeaderList *CASModuleInfo::GenerateHeaders(void)
{
	return new CASHeaderList;
}

CASModuleInfo *CASModuleInfo::FindModuleInfo(ModuleType::Type type)
{
	for (CASModuleInfo *info = m_pHead; info; info = info->m_pNext)
	{
		if (info->m_ModuleType == type)
			return info;
	}

	return NULL;
}

bool CASModuleInfo::SupportsDatabase(DatabaseType::Type type) const
{
	unsigned int shift = (unsigned int)type & DATABASE_SHIFT_COUNT_MASK;

	return ((m_uiDatabaseTypes >> shift) & 1) != 0;
}

bool CASModuleInfo::Validate(CASModuleBuilder *builder)
{
	asIScriptModule *module = builder->m_pModule;

	if (!module)
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASModuleInfo::Validate: module is null!\n");
		return false;
	}

	bool valid = true;

	if (module->GetImportedFunctionCount())
	{
		builder->ReportValidationError("Can't have imported functions!");
		valid = false;
	}

	asUINT typeCount = module->GetObjectTypeCount();

	for (asUINT index = 0; index < typeCount; index++)
	{
		asITypeInfo *type = module->GetObjectTypeByIndex(index);

		if (type->GetFlags() & asOBJ_SHARED)
		{
			char message[VALIDATION_ERROR_BUFFER_LENGTH];
			int length = snprintf(message, sizeof(message), "Can't have shared classes, class '%s::%s' is not allowed!",
				type->GetNamespace(), type->GetName());

			if ((unsigned int)length < sizeof(message))
				builder->ReportValidationError(message);
			else
				CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASModuleInfo::Validate: Failed to format error message!\n");

			valid = false;
		}
	}

	return valid;
}
