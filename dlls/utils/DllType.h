#ifndef DLLTYPE_H
#define DLLTYPE_H

namespace DllType
{
enum Type
{
	NONE = -1,
	CLIENT,
	SERVER
};
}

extern const DllType::Type g_DllType;

const char *DLLToString(DllType::Type type);
DllType::Type StringToDLL(const char *text);

#endif
