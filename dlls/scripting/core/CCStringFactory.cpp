#include "scripting/core/CCStringFactory.h"
#include <string.h>

CCStringFactory::~CCStringFactory()
{
}

const void *CCStringFactory::GetStringConstant(const char *data, asUINT length)
{
	CString value(data, 0, length);
	StringPool_t::iterator it = m_Strings.find(value);

	if (it != m_Strings.end())
		it->second++;
	else
		it = m_Strings.emplace(std::pair<const CString, int>(value, 1)).first;

	return &it->first;
}

int CCStringFactory::ReleaseStringConstant(const void *string)
{
	if (!string)
		return asERROR;

	StringPool_t::iterator it = m_Strings.find(*(const CString *)string);

	if (it == m_Strings.end())
		return asERROR;

	if (it->second-- == 1)
		m_Strings.erase(it);

	return asSUCCESS;
}

int CCStringFactory::GetRawStringData(const void *string, char *data, asUINT *length) const
{
	if (!string)
		return asERROR;

	const CString &value = *(const CString *)string;

	if (length)
		*length = value.Length();

	if (data)
		memmove(data, value.Data(), value.Length());

	return asSUCCESS;
}
