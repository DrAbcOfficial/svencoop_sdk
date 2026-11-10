#ifndef CASCOMMONSERVERMODULEVALIDATION_H
#define CASCOMMONSERVERMODULEVALIDATION_H

class asITypeInfo;
class CASModuleBuilder;

class CASCommonServerModuleValidation
{
public:
	static void PrepareValidator(void);
	static void CleanupValidator(void);
	static bool CheckForEntityHandles(CASModuleBuilder *builder);

private:
	enum
	{
		ENTITY_TYPE_COUNT = 22,
		VALIDATION_ERROR_LENGTH = 256
	};

	static asITypeInfo **m_pEntityObjectTypes;
};

#endif
