#include "extdll.h"
#include "CASExtensionList.h"

bool CStringCaseInsensitiveLess::operator()(const CString &first, const CString &second) const
{
	return stricmp(first.Data(), second.Data()) < 0;
}

CASExtensionList::CASExtensionList()
{
}

CASExtensionList::~CASExtensionList()
{
}

bool CASExtensionList::HasExtension(const char *extension, bool fileName) const
{
	if (!extension || !*extension)
		return false;

	if (fileName)
	{
		const char *dot = extension + strlen(extension) - 1;

		while (dot != extension && *dot != '.')
			dot--;

		if (dot == extension)
			return false;

		extension = dot + 1;
	}

	return m_Extensions.find(CString(extension)) != m_Extensions.end();
}

bool CASExtensionList::AddExtension(const char *extension)
{
	if (!extension || !*extension)
		return false;

	unsigned int length = strlen(extension);

	while (length > 1 && extension[length - 1] == ' ')
		length--;

	while (length > 1 && *extension == ' ')
	{
		extension++;
		length--;
	}

	char *buffer = new char[length + 1];

	memcpy(buffer, extension, length);
	buffer[length] = 0;

	CString value(buffer);

	delete[] buffer;

	if (HasExtension(value.Data(), false))
		return true;

	return m_Extensions.insert(value).second;
}

void CASExtensionList::RemoveExtension(const char *extension)
{
	if (!extension || !*extension)
		return;

	m_Extensions.erase(CString(extension));
}

void CASExtensionList::RemoveAllExtensions(void)
{
	m_Extensions.clear();
}
