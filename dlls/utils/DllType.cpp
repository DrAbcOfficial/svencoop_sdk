#include "extdll.h"
#include "utils/DllType.h"

const DllType::Type g_DllType = DllType::SERVER;

const char *DLLToString(DllType::Type type)
{
	if (type == DllType::CLIENT)
		return "CLIENT";

	if (type == DllType::SERVER)
		return "SERVER";

	return "NONE";
}

DllType::Type StringToDLL(const char *text)
{
	if (!text || !stricmp(text, "NONE"))
		return DllType::NONE;

	if (!stricmp(text, "SERVER"))
		return DllType::SERVER;

	return !stricmp(text, "CLIENT") ? DllType::CLIENT : DllType::NONE;
}
