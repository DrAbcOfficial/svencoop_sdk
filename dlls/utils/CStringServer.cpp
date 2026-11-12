#include "extdll.h"
#include "enginecallback.h"
#include "CString.h"
#include <ctype.h>

namespace String
{
	const CompareType DEFAULT_COMPARE = CaseSensitive;

	int CompareCharacters(char left, char right, CompareType compareType)
	{
		return compareType == CaseSensitive ? left - right : tolower(left) - tolower(right);
	}

	int Compare(const char *left, const char *right, CompareType compareType)
	{
		return compareType == CaseSensitive ? strcmp(left, right) : stricmp(left, right);
	}

	int CompareN(const char *left, const char *right, unsigned int length, CompareType compareType)
	{
		return compareType == CaseSensitive ? strncmp(left, right, length) : strnicmp(left, right, length);
	}

	char BoolToCharacter(bool value)
	{
		return value + '0';
	}

	char *TrimTrailingCharacters(char *text, char character, unsigned int length)
	{
		unsigned int size = strlen(text);

		if (length > size)
			length = size;

		if (length)
		{
			for (unsigned int i = length - 1; i && text[i] == character; i--)
				text[i] = 0;
		}

		return text;
	}

	char *TrimTrailingCharacters(char *text, char character)
	{
		return TrimTrailingCharacters(text, character, strlen(text));
	}

	char *TrimLeadingCharacters(char *text, char character, unsigned int length)
	{
		unsigned int size = strlen(text);

		if (length > size)
			length = size;

		if (length)
		{
			char *start = text;
			char *end = text + length - 1;

			while (start != end && *start == character)
				start++;

			if (start != text)
				memmove(text, start, size - (start - text) + 1);
		}

		return text;
	}

	char *TrimLeadingCharacters(char *text, char character)
	{
		return TrimLeadingCharacters(text, character, strlen(text));
	}

	char *TrimCharacters(char *text, char character)
	{
		TrimTrailingCharacters(text, character);
		return TrimLeadingCharacters(text, character);
	}
}

const unsigned int CString::INVALID_INDEX = ~0u;
const unsigned int CString::NO_MORE_TOKENS = ~1u;
const CString CString::EMPTY_STRING("");
const CString CString::WHITESPACE_CHARACTERS(" \t\n\r\v");

namespace
{
	enum
	{
		NUMERIC_BUFFER_LENGTH = 512,
		FORMAT_BUFFER_LENGTH = 16384
	};

	void AssignFloatingPoint(CString &text, double value)
	{
		char buffer[NUMERIC_BUFFER_LENGTH];
		unsigned int length = _snprintf(buffer, sizeof(buffer), "%f", value);

		String::TrimTrailingCharacters(buffer, '0', length);
		String::TrimTrailingCharacters(buffer, '.', length);
		text.Assign(buffer, strlen(buffer));
	}
}

CString::CString(char value)
{
	char text[] = { value, 0 };

	Construct();
	*this = text;
}

CString::CString(bool value)
{
	char text[] = { String::BoolToCharacter(value), 0 };

	Construct();
	*this = text;
}

CString::CString(int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	Construct();

	unsigned int length = _snprintf(buffer, sizeof(buffer), "%d", value);

	Assign(buffer, length);
}

CString::CString(unsigned int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	Construct();

	unsigned int length = _snprintf(buffer, sizeof(buffer), "%u", value);

	Assign(buffer, length);
}

CString::CString(long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	Construct();

	unsigned int length = _snprintf(buffer, sizeof(buffer), "%lld", value);

	Assign(buffer, length);
}

CString::CString(unsigned long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	Construct();

	unsigned int length = _snprintf(buffer, sizeof(buffer), "%llu", value);

	Assign(buffer, length);
}

CString::CString(float value)
{
	Construct();
	AssignFloatingPoint(*this, value);
}

CString::CString(double value)
{
	Construct();
	AssignFloatingPoint(*this, value);
}

CString::CString(char *text, bool takeOwnership)
{
	Construct();

	if (takeOwnership)
		TakeOwnership(text);
	else
		*this = text;
}

CString::CString(const char *text, unsigned int start, unsigned int length)
{
	Construct();
	Assign(text, start, length);
}

CString::CString(const CString &text, unsigned int start, unsigned int length)
{
	Construct();
	Assign(text.Data(), start, length);
}

CString &CString::operator=(char value)
{
	char text[] = { value, 0 };

	Assign(text, strlen(text));
	return *this;
}

CString &CString::operator=(bool value)
{
	return *this = String::BoolToCharacter(value);
}

CString &CString::operator=(int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];
	unsigned int length = _snprintf(buffer, sizeof(buffer), "%d", value);

	Assign(buffer, length);
	return *this;
}

CString &CString::operator=(unsigned int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];
	unsigned int length = _snprintf(buffer, sizeof(buffer), "%u", value);

	Assign(buffer, length);
	return *this;
}

CString &CString::operator=(long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];
	unsigned int length = _snprintf(buffer, sizeof(buffer), "%lld", value);

	Assign(buffer, length);
	return *this;
}

CString &CString::operator=(unsigned long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];
	unsigned int length = _snprintf(buffer, sizeof(buffer), "%llu", value);

	Assign(buffer, length);
	return *this;
}

CString &CString::operator=(float value)
{
	return *this = (double)value;
}

CString &CString::operator=(double value)
{
	AssignFloatingPoint(*this, value);
	return *this;
}

CString &CString::TakeOwnership(char *text)
{
	if (m_pszString && m_pszString != m_szBuffer)
	{
		delete[] m_pszString;
		m_pszString = NULL;
	}

	if (text)
	{
		m_uiLength = strlen(text);
		m_uiTokenizeIndex = 0;
		m_pszString = text;
		m_uiCapacity = m_uiLength + 1;
	}
	else
	{
		ALERT(at_error, "CString::TakeOwnership: null pointer passed; can't take ownership!\n");
		*this = "";
	}

	return *this;
}

unsigned int CString::RecalculateLength(void)
{
	unsigned int previousLength = m_uiLength;

	m_uiLength = strlen(m_pszString);

	if (m_uiLength != previousLength)
		m_uiTokenizeIndex = 0;

	return m_uiLength;
}

unsigned int CString::GetCapacity(void) const
{
	return m_uiCapacity;
}

void CString::Clear(void)
{
	Reserve(0, true);
	m_pszString[0] = 0;
	m_uiLength = 0;
	m_uiTokenizeIndex = 0;
}

void CString::SetStaticString(char *text, unsigned int length)
{
	m_pszString = text;
	m_uiLength = length == DEFAULT_LENGTH ? strlen(text) : length;
	m_bStatic = true;
}

CString &CString::Assign(const char *text, unsigned int start, unsigned int length)
{
	Clear();
	return Append(text, start, length);
}

CString &CString::Assign(const CString &text, unsigned int start, unsigned int length)
{
	return Assign(text.Data(), start, length);
}

void CString::Append(const char *text, unsigned int length)
{
	if (!length)
		return;

	Reserve(m_uiLength + length, true);
	strcpy(m_pszString + m_uiLength, text);
	m_uiLength += length;
	m_uiTokenizeIndex = 0;
}

CString &CString::Append(const char *text, unsigned int start, unsigned int length)
{
	if (!*text)
		return *this;

	unsigned int sourceLength = strlen(text);

	if (start >= sourceLength)
		start = sourceLength - 1;

	if (length == DEFAULT_LENGTH || length > sourceLength - start)
		length = sourceLength - start;

	Reserve(m_uiLength + length, true);

	char *output = m_pszString + m_uiLength;
	const char *input = text + start;

	m_uiLength += length;

	while (length--)
		*output++ = *input++;

	*output = 0;
	return *this;
}

CString &CString::Append(const CString &text, unsigned int start, unsigned int length)
{
	return Append(text.Data(), start, length);
}

CString &CString::operator+=(const char *text)
{
	Append(text, strlen(text));
	return *this;
}

CString &CString::operator+=(const CString &text)
{
	Append(text.Data(), text.Length());
	return *this;
}

CString &CString::operator+=(char value)
{
	char text[] = { value, 0 };

	Append(text, strlen(text));
	return *this;
}

CString &CString::operator+=(bool value)
{
	return *this += String::BoolToCharacter(value);
}

CString &CString::operator+=(int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%i", value);
	Append(buffer, strlen(buffer));
	return *this;
}

CString &CString::operator+=(unsigned int value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%u", value);
	Append(buffer, strlen(buffer));
	return *this;
}

CString &CString::operator+=(long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%lli", value);
	Append(buffer, strlen(buffer));
	return *this;
}

CString &CString::operator+=(unsigned long long value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];

	_snprintf(buffer, sizeof(buffer), "%llu", value);
	Append(buffer, strlen(buffer));
	return *this;
}

CString &CString::operator+=(float value)
{
	return *this += (double)value;
}

CString &CString::operator+=(double value)
{
	char buffer[NUMERIC_BUFFER_LENGTH];
	unsigned int length = _snprintf(buffer, sizeof(buffer), "%f", value);

	String::TrimTrailingCharacters(buffer, '0', length);
	String::TrimTrailingCharacters(buffer, '.', length);
	Append(buffer, strlen(buffer));
	return *this;
}

char &CString::operator[](unsigned int index)
{
	return m_pszString[index];
}

const char &CString::operator[](unsigned int index) const
{
	return m_pszString[index];
}

bool CString::IsEmpty(void) const
{
	return m_uiLength == 0;
}

void CString::ClearMemory(void)
{
	if (!GetDynamicAllocation())
		return;

	delete[] m_pszString;
	Construct();
}

char CString::CharAt(unsigned int index) const
{
	return m_pszString[index < m_uiLength ? index : 0];
}

void CString::SetCharAt(unsigned int index, char value)
{
	if (index < m_uiLength)
		m_pszString[index] = value;
}

int CString::Compare(const char *text) const
{
	int result = strcmp(m_pszString, text);

	return result < 0 ? -1 : result > 0 ? 1 : 0;
}

int CString::Compare(const CString &text) const
{
	return Compare(text.Data());
}

int CString::CompareN(const char *text, unsigned int length) const
{
	return strncmp(m_pszString, text, length);
}

int CString::CompareN(const CString &text, unsigned int length) const
{
	return CompareN(text.Data(), length);
}

int CString::ICompare(const char *text) const
{
	return stricmp(m_pszString, text);
}

int CString::ICompare(const CString &text) const
{
	return ICompare(text.Data());
}

int CString::ICompareN(const char *text, unsigned int length) const
{
	return strnicmp(m_pszString, text, length);
}

int CString::ICompareN(const CString &text, unsigned int length) const
{
	return ICompareN(text.Data(), length);
}

bool CString::operator==(const char *text) const
{
	return !strcmp(m_pszString, text);
}

bool CString::operator==(const CString &text) const
{
	return *this == text.Data();
}

bool CString::operator!=(const char *text) const
{
	return strcmp(m_pszString, text) != 0;
}

bool CString::operator!=(const CString &text) const
{
	return *this != text.Data();
}

bool CString::StartsWith(const char *text, String::CompareType compareType) const
{
	return !String::CompareN(m_pszString, text, strlen(text), compareType);
}

bool CString::StartsWith(const CString &text, String::CompareType compareType) const
{
	return StartsWith(text.Data(), compareType);
}

bool CString::EndsWith(const char *text, String::CompareType compareType) const
{
	unsigned int length = strlen(text);

	return length <= m_uiLength && !String::CompareN(m_pszString + m_uiLength - length, text, length, compareType);
}

bool CString::EndsWith(const CString &text, String::CompareType compareType) const
{
	return EndsWith(text.Data(), compareType);
}

unsigned int CString::Find(char character, unsigned int start, String::CompareType compareType) const
{
	for (unsigned int i = start; i < m_uiLength; i++)
	{
		if (!String::CompareCharacters(character, m_pszString[i], compareType))
			return i;
	}

	return INVALID_INDEX;
}

unsigned int CString::Find(const char *text, unsigned int start, String::CompareType compareType) const
{
	unsigned int length = strlen(text);

	if (length > m_uiLength || start >= m_uiLength - length + 1)
		return INVALID_INDEX;

	for (unsigned int i = start; i < m_uiLength - length + 1; i++)
	{
		if (!String::CompareN(m_pszString + i, text, length, compareType))
			return i;
	}

	return INVALID_INDEX;
}

unsigned int CString::Find(const CString &text, unsigned int start, String::CompareType compareType) const
{
	return Find(text.Data(), start, compareType);
}

unsigned int CString::RFind(char character, unsigned int start, String::CompareType compareType) const
{
	if (!m_uiLength || start >= m_uiLength)
		return INVALID_INDEX;

	unsigned int i = m_uiLength - start;

	do
	{
		i--;

		if (!String::CompareCharacters(character, m_pszString[i], compareType))
			return i;
	}
	while (i);

	return INVALID_INDEX;
}

unsigned int CString::RFind(const char *text, unsigned int start, String::CompareType compareType) const
{
	if (!*text)
		return INVALID_INDEX;

	unsigned int length = strlen(text);

	if (length > m_uiLength || start >= m_uiLength - length + 1)
		return INVALID_INDEX;

	unsigned int i = m_uiLength - length + 1 - start;

	do
	{
		i--;

		if (!String::CompareN(m_pszString + i, text, length, compareType))
			return i;
	}
	while (i);

	return INVALID_INDEX;
}

unsigned int CString::RFind(const CString &text, unsigned int start, String::CompareType compareType) const
{
	return RFind(text.Data(), start, compareType);
}

unsigned int CString::FindFirstOf(const char *text, unsigned int start, String::CompareType compareType) const
{
	return FindFirstOf(CString(text), start, compareType);
}

unsigned int CString::FindFirstOf(const CString &text, unsigned int start, String::CompareType compareType) const
{
	if (text.IsEmpty())
		return INVALID_INDEX;

	for (unsigned int i = start; i < m_uiLength; i++)
	{
		// compareType is used as the offset within the character set
		if (text.Find(m_pszString[i], compareType) != INVALID_INDEX)
			return i;
	}

	return INVALID_INDEX;
}

unsigned int CString::FindLastOf(const char *text, unsigned int start, String::CompareType compareType) const
{
	return FindLastOf(CString(text), start, compareType);
}

unsigned int CString::FindLastOf(const CString &text, unsigned int start, String::CompareType compareType) const
{
	if (text.IsEmpty() || !m_uiLength || start >= m_uiLength)
		return INVALID_INDEX;

	unsigned int i = m_uiLength - start;

	do
	{
		i--;

		if (text.Find(m_pszString[i], compareType) != INVALID_INDEX)
			return i;
	}
	while (i);

	return INVALID_INDEX;
}

unsigned int CString::FindFirstNotOf(const char *text, unsigned int start, String::CompareType compareType) const
{
	if (!text)
		return INVALID_INDEX;

	return FindFirstNotOf(CString(text), start, compareType);
}

unsigned int CString::FindFirstNotOf(const CString &text, unsigned int start, String::CompareType compareType) const
{
	if (text.IsEmpty() || !m_uiLength || start >= m_uiLength)
		return INVALID_INDEX;

	const char *position = m_pszString + start;

	while (*position && text.FindFirstOf(CString(*position), 0, compareType) != INVALID_INDEX)
		position++;

	return *position ? position - m_pszString : INVALID_INDEX;
}

unsigned int CString::FindLastNotOf(const char *text, unsigned int start, String::CompareType compareType) const
{
	if (!text)
		return INVALID_INDEX;

	return FindLastNotOf(CString(text), start, compareType);
}

unsigned int CString::FindLastNotOf(const CString &text, unsigned int start, String::CompareType compareType) const
{
	// The reverse exclusion search ignores compareType
	(void) compareType;

	if (text.IsEmpty() || !m_uiLength)
		return INVALID_INDEX;

	if (start == INVALID_INDEX)
		start = m_uiLength - 1;

	if (start >= m_uiLength)
		return INVALID_INDEX;

	for (unsigned int i = start;; i--)
	{
		if (text.FindLastOf(CString(m_pszString[i])) == INVALID_INDEX)
			return i;

		if (!i)
			return INVALID_INDEX;
	}
}

void CString::Trim(char character)
{
	String::TrimCharacters(m_pszString, character);
	m_uiLength = strlen(m_pszString);
	m_uiTokenizeIndex = 0;
}

CString &CString::ToLowercase(void)
{
	for (unsigned int i = 0; i < m_uiLength; i++)
		m_pszString[i] = tolower(m_pszString[i]);

	return *this;
}

CString &CString::ToUppercase(void)
{
	for (unsigned int i = 0; i < m_uiLength; i++)
		m_pszString[i] = toupper(m_pszString[i]);

	return *this;
}

void CString::Truncate(unsigned int length)
{
	if (length < m_uiLength)
	{
		memset(m_pszString + length, 0, m_uiLength - length);
		m_uiLength = length;
	}
}

void CString::VFormat(const char *format, va_list arguments)
{
	char buffer[FORMAT_BUFFER_LENGTH];
	unsigned int length = vsnprintf(buffer, sizeof(buffer), format, arguments);

	buffer[sizeof(buffer) - 1] = 0;

	if (length >= sizeof(buffer))
	{
		ALERT(at_error, "Error formatting string: format used: '%s'\n", format);
		return;
	}

	*this = buffer;
}

void CString::Format(const char *format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	VFormat(format, arguments);
	va_end(arguments);
}

CString CString::SubString(unsigned int start, unsigned int length) const
{
	return CString(m_pszString, start, length);
}

CString CString::Tokenize(const CString &delimiter) const
{
	if (!delimiter.Length())
		return CString(INVALID_INDEX);

	CString remaining = SubString(m_uiTokenizeIndex);

	if (!m_uiLength || !remaining.Length() || m_uiTokenizeIndex >= m_uiLength)
	{
		m_uiTokenizeIndex = 0;
		return CString(NO_MORE_TOKENS);
	}

	unsigned int position = remaining.FindFirstOf(delimiter, 0, String::CaseSensitive);

	if (position == INVALID_INDEX)
	{
		m_uiTokenizeIndex += remaining.Length();
		return CString(remaining);
	}

	CString token = remaining.SubString(0, position);
	unsigned int advance = position + delimiter.Length();

	if (advance > m_uiLength)
		advance = m_uiLength;

	m_uiTokenizeIndex += advance;
	return CString(token);
}

CString CString::Tokenize(const char *delimiter) const
{
	return delimiter ? Tokenize(CString(delimiter)) : CString(INVALID_INDEX);
}

CString CString::Tokenize(char delimiter) const
{
	return delimiter ? Tokenize(CString(delimiter)) : CString(INVALID_INDEX);
}

CString &CString::Replace(char substring, char replacement)
{
	if (!substring)
		return *this;

	char *end = m_pszString + m_uiLength;

	for (char *position = m_pszString; position != end; position++)
	{
		if (*position == substring)
			*position = replacement;

		// The loop stops on a NUL replacement even if this byte did not match
		if (!replacement)
		{
			m_uiLength = position - m_pszString;
			break;
		}
	}

	return *this;
}

CString &CString::Replace(const char *substring, const char *replacement, String::CompareType compareType)
{
	if (!*substring)
		return *this;

	CString result;
	unsigned int length = strlen(substring);
	const char *end = m_pszString + m_uiLength;

	for (const char *position = m_pszString; position != end; position++)
	{
		if (String::CompareN(position, substring, length, compareType))
			result += *position;
		else
		{
			result += replacement;
			position += length - 1;
		}
	}

	Assign(result.Data(), result.Length());
	return *this;
}

CString &CString::Replace(const CString &substring, const CString &replacement, String::CompareType compareType)
{
	return Replace(substring.Data(), replacement.Data(), compareType);
}

CString operator+(const CString &left, const char *right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, const CString &right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, char right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, bool right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, int right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, unsigned int right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, long long right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, unsigned long long right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, float right)
{
	return CString(left) += right;
}

CString operator+(const CString &left, double right)
{
	return CString(left) += right;
}
