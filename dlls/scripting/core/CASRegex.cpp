#include "scripting/core/CASRegex.h"
#include "CString.h"

CASRegex::CASRegex()
{
}

CASRegex::CASRegex(const CASRegex *other)
	: m_Regex(other->m_Regex)
{
	other->Release();
}

CASRegex::CASRegex(const CString &expression, int flags)
	: m_Regex(std::string(expression.Data()), (std::regex::flag_type)flags)
{
}

CASRegex::~CASRegex()
{
}

void CASRegex::Release(void) const
{
	if (InternalRelease())
		delete this;
}

CASRegex &CASRegex::Assign(const CASRegex *other)
{
	if (other)
	{
		if (this != other)
			m_Regex = other->m_Regex;

		other->Release();
	}

	return *this;
}

CASRegex &CASRegex::Assign(const CString &expression, int flags)
{
	m_Regex.assign(std::string(expression.Data()), (std::regex::flag_type)flags);
	return *this;
}

unsigned int CASRegex::GetMarkCount(void) const
{
	return m_Regex.mark_count();
}

int CASRegex::GetFlags(void) const
{
	return m_Regex.flags();
}

void CASRegex::Swap(CASRegex *other)
{
	if (other)
	{
		if (this != other)
			m_Regex.swap(other->m_Regex);

		other->Release();
	}
}

CASRegexSubMatch::CASRegexSubMatch(const std::csub_match &match)
	: m_pMatch(&match)
{
}

CASRegexSubMatch::CASRegexSubMatch(const CASRegexSubMatch &other)
	: m_pMatch(other.m_pMatch)
{
}

CASRegexSubMatch::~CASRegexSubMatch()
{
}

int CASRegexSubMatch::GetLength(void) const
{
	return m_pMatch->length();
}

CString CASRegexSubMatch::GetString(void) const
{
	return CString(m_pMatch->str().c_str());
}

int CASRegexSubMatch::Compare(const CASRegexSubMatch &other) const
{
	return m_pMatch->compare(*other.m_pMatch);
}

int CASRegexSubMatch::Compare(const CString &text) const
{
	return m_pMatch->compare(text.Data());
}

CASRegexMatchResults::CASRegexMatchResults()
{
}

CASRegexMatchResults::CASRegexMatchResults(const CASRegexMatchResults &other)
	: m_Matches(other.m_Matches)
{
}

CASRegexMatchResults::~CASRegexMatchResults()
{
}

void CASRegexMatchResults::Release(void) const
{
	if (InternalRelease())
		delete this;
}

bool CASRegexMatchResults::IsEmpty(void) const
{
	return m_Matches.empty();
}

size_t CASRegexMatchResults::GetSize(void) const
{
	return m_Matches.size();
}

size_t CASRegexMatchResults::GetMaxSize(void) const
{
	return m_Matches.max_size();
}

CASRegexSubMatch CASRegexMatchResults::operator[](size_t index) const
{
	return CASRegexSubMatch(m_Matches[index]);
}

CASRegexSubMatch CASRegexMatchResults::GetPrefix(void) const
{
	return CASRegexSubMatch(m_Matches.prefix());
}

CASRegexSubMatch CASRegexMatchResults::GetSuffix(void) const
{
	return CASRegexSubMatch(m_Matches.suffix());
}

int CASRegexMatchResults::GetLength(size_t index) const
{
	return m_Matches.length(index);
}

int CASRegexMatchResults::GetPosition(size_t index) const
{
	return m_Matches.position(index);
}

CString CASRegexMatchResults::GetString(size_t index) const
{
	return CString(m_Matches.str(index).c_str());
}

CString CASRegexMatchResults::Format(const CString &format, int flags) const
{
	std::string result = m_Matches.format(std::string(format.Data()),
		(std::regex_constants::match_flag_type)flags);

	return CString(result.c_str());
}

void CASRegexMatchResults::Swap(CASRegexMatchResults *other)
{
	if (other && this != other)
		m_Matches.swap(other->m_Matches);
}
