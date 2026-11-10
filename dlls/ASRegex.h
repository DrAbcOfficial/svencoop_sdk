#ifndef ASREGEX_H
#define ASREGEX_H

#include <regex>

class CASDocumentation;
class CASRegex;
class CASRegexMatchResults;
class CString;

bool ASRegexMatch(const CString &text, const CASRegex *expression,
	std::regex_constants::match_flag_type flags);
bool ASRegexMatch(const CString &text, CASRegexMatchResults *results,
	const CASRegex *expression, std::regex_constants::match_flag_type flags);
bool ASRegexSearch(const CString &text, const CASRegex *expression,
	std::regex_constants::match_flag_type flags);
bool ASRegexSearch(const CString &text, CASRegexMatchResults *results,
	const CASRegex *expression, std::regex_constants::match_flag_type flags);
CString ASRegexReplace(const CString &text, const CASRegex *expression,
	const CString &format, std::regex_constants::match_flag_type flags);

void RegisterSCScriptRegexConstants(CASDocumentation *documentation);

void RegisterSCScriptRegex(CASDocumentation *documentation);

#endif
