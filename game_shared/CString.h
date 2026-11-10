#ifndef SVEN_CSTRING_H
#define SVEN_CSTRING_H

#include <stdarg.h>

namespace String
{
	enum CompareType
	{
		CaseSensitive = 0,
		CaseInsensitive
	};

	extern const CompareType DEFAULT_COMPARE;

	int CompareCharacters(char left, char right, CompareType compareType);
	int Compare(const char *left, const char *right, CompareType compareType);
	int CompareN(const char *left, const char *right, unsigned int length, CompareType compareType);
	char BoolToCharacter(bool value);
	char *TrimTrailingCharacters(char *text, char character, unsigned int length);
	char *TrimTrailingCharacters(char *text, char character);
	char *TrimLeadingCharacters(char *text, char character, unsigned int length);
	char *TrimLeadingCharacters(char *text, char character);
	char *TrimCharacters(char *text, char character);
}

class CString
{
public:
	CString();
	CString(const char *text);
	CString(const CString &text);

	CString(char value);
	CString(bool value);
	CString(int value);
	CString(unsigned int value);
	CString(long long value);
	CString(unsigned long long value);
	CString(float value);
	CString(double value);
	CString(char *text, bool takeOwnership);
	CString(const char *text, unsigned int start, unsigned int length);
	CString(const CString &text, unsigned int start, unsigned int length);

	~CString();

	CString &operator=(const char *text);
	CString &operator=(const CString &text);

	void Construct(void);
	unsigned int GetDynamicAllocation(void) const;
	void Resize(unsigned int length, bool preserve);
	void Reserve(unsigned int length, bool preserve);
	void Assign(const char *text, unsigned int length);
	const char *Data(void) const;
	unsigned int Length(void) const;

	static const unsigned int DEFAULT_LENGTH = ~0u;
	static const unsigned int INVALID_INDEX;
	static const unsigned int NO_MORE_TOKENS;

	static const CString EMPTY_STRING;
	static const CString WHITESPACE_CHARACTERS;

	CString &TakeOwnership(char *text);
	CString &operator=(char value);
	CString &operator=(bool value);
	CString &operator=(int value);
	CString &operator=(unsigned int value);
	CString &operator=(long long value);
	CString &operator=(unsigned long long value);
	CString &operator=(float value);
	CString &operator=(double value);
	unsigned int RecalculateLength(void);
	unsigned int GetCapacity(void) const;
	void Clear(void);
	void SetStaticString(char *text, unsigned int length = DEFAULT_LENGTH);
	CString &Assign(const char *text, unsigned int start, unsigned int length);
	CString &Assign(const CString &text, unsigned int start, unsigned int length);
	void Append(const char *text, unsigned int length);
	CString &Append(const char *text, unsigned int start, unsigned int length);
	CString &Append(const CString &text, unsigned int start, unsigned int length);
	CString &operator+=(const char *text);
	CString &operator+=(const CString &text);
	CString &operator+=(char value);
	CString &operator+=(bool value);
	CString &operator+=(int value);
	CString &operator+=(unsigned int value);
	CString &operator+=(long long value);
	CString &operator+=(unsigned long long value);
	CString &operator+=(float value);
	CString &operator+=(double value);
	char &operator[](unsigned int index);
	const char &operator[](unsigned int index) const;
	bool IsEmpty(void) const;
	void ClearMemory(void);
	char CharAt(unsigned int index) const;
	void SetCharAt(unsigned int index, char value);

	int Compare(const char *text) const;
	int Compare(const CString &text) const;
	int CompareN(const char *text, unsigned int length) const;
	int CompareN(const CString &text, unsigned int length) const;
	int ICompare(const char *text) const;
	int ICompare(const CString &text) const;
	int ICompareN(const char *text, unsigned int length) const;
	int ICompareN(const CString &text, unsigned int length) const;
	bool operator==(const char *text) const;
	bool operator==(const CString &text) const;
	bool operator!=(const char *text) const;
	bool operator!=(const CString &text) const;
	bool StartsWith(const char *text, String::CompareType compareType = String::DEFAULT_COMPARE) const;
	bool StartsWith(const CString &text, String::CompareType compareType = String::DEFAULT_COMPARE) const;
	bool EndsWith(const char *text, String::CompareType compareType = String::DEFAULT_COMPARE) const;
	bool EndsWith(const CString &text, String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int Find(char character, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int Find(const char *text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int Find(const CString &text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int RFind(char character, unsigned int start = INVALID_INDEX,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int RFind(const char *text, unsigned int start = INVALID_INDEX,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int RFind(const CString &text, unsigned int start = INVALID_INDEX,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindFirstOf(const char *text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindFirstOf(const CString &text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindLastOf(const char *text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindLastOf(const CString &text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindFirstNotOf(const char *text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindFirstNotOf(const CString &text, unsigned int start = 0,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindLastNotOf(const char *text, unsigned int start = INVALID_INDEX,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;
	unsigned int FindLastNotOf(const CString &text, unsigned int start = INVALID_INDEX,
		String::CompareType compareType = String::DEFAULT_COMPARE) const;

	void Trim(char character = ' ');
	void VFormat(const char *format, va_list arguments);
	void Format(const char *format, ...);
	CString SubString(unsigned int start = 0, unsigned int length = INVALID_INDEX) const;
	CString Tokenize(const CString &delimiter) const;
	CString Tokenize(const char *delimiter) const;
	CString Tokenize(char delimiter) const;
	CString &Replace(char substring, char replacement);
	CString &Replace(const char *substring, const char *replacement,
		String::CompareType compareType = String::DEFAULT_COMPARE);
	CString &Replace(const CString &substring, const CString &replacement,
		String::CompareType compareType = String::DEFAULT_COMPARE);
	CString &ToLowercase(void);
	CString &ToUppercase(void);
	void Truncate(unsigned int length);

private:
	char					m_szBuffer[20];
	char					*m_pszString;
	unsigned int			m_uiLength;
	unsigned int			m_uiCapacity : 31;
	unsigned int			m_bStatic : 1;
	mutable unsigned int	m_uiTokenizeIndex;
};

CString operator+(const CString &left, const char *right);
CString operator+(const CString &left, const CString &right);
CString operator+(const CString &left, char right);
CString operator+(const CString &left, bool right);
CString operator+(const CString &left, int right);
CString operator+(const CString &left, unsigned int right);
CString operator+(const CString &left, long long right);
CString operator+(const CString &left, unsigned long long right);
CString operator+(const CString &left, float right);
CString operator+(const CString &left, double right);

#endif
