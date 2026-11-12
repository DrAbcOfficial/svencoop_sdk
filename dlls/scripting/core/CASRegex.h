#ifndef CASREGEX_H
#define CASREGEX_H

#include "scripting/core/CASBaseClass.h"
#include <regex>

class CString;
class CASRegexMatchResults;

class CASRegex : public CASRefCountedBaseClass
{
public:
	CASRegex();
	CASRegex(const CASRegex *other);
	CASRegex(const CString &expression, int flags);
	~CASRegex();

	void Release(void) const;
	CASRegex &Assign(const CASRegex *other);
	CASRegex &Assign(const CString &expression, int flags);
	unsigned int GetMarkCount(void) const;
	int GetFlags(void) const;
	void Swap(CASRegex *other);

private:
	friend bool ASRegexMatch(const CString &text, const CASRegex *expression,
		std::regex_constants::match_flag_type flags);
	friend bool ASRegexMatch(const CString &text, CASRegexMatchResults *results,
		const CASRegex *expression, std::regex_constants::match_flag_type flags);
	friend bool ASRegexSearch(const CString &text, const CASRegex *expression,
		std::regex_constants::match_flag_type flags);
	friend bool ASRegexSearch(const CString &text, CASRegexMatchResults *results,
		const CASRegex *expression, std::regex_constants::match_flag_type flags);
	friend CString ASRegexReplace(const CString &text, const CASRegex *expression,
		const CString &format, std::regex_constants::match_flag_type flags);

	std::regex m_Regex;
};

class CASRegexSubMatch
{
public:
	CASRegexSubMatch(const std::csub_match &match);
	CASRegexSubMatch(const CASRegexSubMatch &other);
	~CASRegexSubMatch();

	int GetLength(void) const;
	CString GetString(void) const;
	int Compare(const CASRegexSubMatch &other) const;
	int Compare(const CString &text) const;

private:
	const std::csub_match *m_pMatch;
};

class CASRegexMatchResults : public CASRefCountedBaseClass
{
public:
	CASRegexMatchResults();
	CASRegexMatchResults(const CASRegexMatchResults &other);
	~CASRegexMatchResults();

	void Release(void) const;
	bool IsEmpty(void) const;
	size_t GetSize(void) const;
	size_t GetMaxSize(void) const;
	CASRegexSubMatch operator[](size_t index) const;
	CASRegexSubMatch GetPrefix(void) const;
	CASRegexSubMatch GetSuffix(void) const;
	int GetLength(size_t index = 0) const;
	int GetPosition(size_t index = 0) const;
	CString GetString(size_t index = 0) const;
	CString Format(const CString &format, int flags) const;
	void Swap(CASRegexMatchResults *other);

private:
	friend bool ASRegexMatch(const CString &text, CASRegexMatchResults *results,
		const CASRegex *expression, std::regex_constants::match_flag_type flags);
	friend bool ASRegexSearch(const CString &text, CASRegexMatchResults *results,
		const CASRegex *expression, std::regex_constants::match_flag_type flags);

	std::cmatch m_Matches;
};

#endif
