#ifndef CASMODULEBUILDER_H
#define CASMODULEBUILDER_H

#include "CASBaseClass.h"
#include "CASDirectoryList.h"

class CASHeaderList;
class CASModule;
class CASModuleInfo;
class CScriptBuilder;
class asIScriptModule;

class CASModuleBuilder : public CASBaseClass
{
public:
	CASModuleBuilder(ModuleType::Type type, const char *name);
	~CASModuleBuilder();
	bool Build(CASModule **module);

	int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder);

	bool FormatScriptFilename(const char *filename, char *scriptPath, unsigned int scriptPathSize,
		char *localPath, unsigned int localPathSize);
	int AddScriptFromFile(const char *filename);
	const char *GetModuleName(void) const;
	int AddScriptFromBuffer(const char *name, const char *source);
	void ReportValidationError(const char *message) const;

private:
	friend class CASModuleInfo;
	friend class CASCommonServerModuleValidation;
	friend class CASBaseManager;

	CASModuleInfo	*m_pModuleInfo;
	CScriptBuilder	*m_pScriptBuilder;
	asIScriptModule	*m_pModule;
	unsigned int	m_uiIncludeErrors;
	bool			m_bBuilt;
	unsigned int	m_uiScriptCount;
	CASHeaderList	*m_pHeaders;
};

int IncludeCallback(const char *include, const char *from, CScriptBuilder *builder, void *userData);

#endif
