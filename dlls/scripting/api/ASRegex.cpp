#include "scripting/api/ASRegex.h"
#include "scripting/core/CASRegex.h"
#include "scripting/core/CASDocumentation.h"
#include "CString.h"
#include "scripting/core/CASServerManager.h"
#include <new>

bool ASRegexMatch(const CString &text, const CASRegex *expression,
	std::regex_constants::match_flag_type flags)
{
	if (!expression)
		return false;

	bool matched = std::regex_match(text.Data(), expression->m_Regex, flags);

	expression->Release();
	return matched;
}

bool ASRegexMatch(const CString &text, CASRegexMatchResults *results,
	const CASRegex *expression, std::regex_constants::match_flag_type flags)
{
	bool matched = false;

	if (results)
	{
		if (expression)
			matched = std::regex_match(text.Data(), results->m_Matches, expression->m_Regex, flags);

		results->Release();
	}

	if (expression)
		expression->Release();

	return matched;
}

bool ASRegexSearch(const CString &text, const CASRegex *expression,
	std::regex_constants::match_flag_type flags)
{
	if (!expression)
		return false;

	bool matched = std::regex_search(text.Data(), expression->m_Regex, flags);

	expression->Release();
	return matched;
}

bool ASRegexSearch(const CString &text, CASRegexMatchResults *results,
	const CASRegex *expression, std::regex_constants::match_flag_type flags)
{
	bool matched = false;

	if (results)
	{
		if (expression)
			matched = std::regex_search(text.Data(), results->m_Matches, expression->m_Regex, flags);

		results->Release();
	}

	if (expression)
		expression->Release();

	return matched;
}

CString ASRegexReplace(const CString &text, const CASRegex *expression,
	const CString &format, std::regex_constants::match_flag_type flags)
{
	if (!expression)
		return CString("");

	std::string replacement = std::regex_replace(std::string(text.Data()), expression->m_Regex,
		std::string(format.Data()), flags);
	CString result(replacement.c_str());

	expression->Release();
	return result;
}


namespace
{
	CASRegex *DefaultConstructRegex()
	{
		return new CASRegex();
	}

	CASRegex *CopyConstructRegex(const CASRegex *other)
	{
		return new CASRegex(other);
    }

    CASRegex *InitializeConstructRegex(const CString &expression, int flags)
    {
	    return new CASRegex(expression, flags);
    }

    void DefaultConstructRegexSubMatch(void *memory)
    {
	    CASServerManager::GetInstance()->SetExceptionOnActiveContext("Cannot use default constructor for SubMatch!", true);
    }

    void CopyConstructRegexSubMatch(const CASRegexSubMatch &other, void *memory)
    {
	    new (memory) CASRegexSubMatch(other);
    }

    void DestructRegexSubMatch(CASRegexSubMatch *match)
    {
	    match->~CASRegexSubMatch();
    }

    CASRegexMatchResults *ConstructRegexMatchResults()
    {
	    return new CASRegexMatchResults();
    }

    CASRegexMatchResults *CopyConstructRegexMatchResults(const CASRegexMatchResults *other)
    {
	    return new CASRegexMatchResults(*other);
    }
    }

    void RegisterSCScriptRegexConstants(CASDocumentation *documentation)
    {
	    documentation->RegisterEnum(
	        "Regular expression flags. These can be bitwise OR'd together to combine options.", "FlagType");
	    documentation->RegisterEnumValue("", "FlagType", "icase", std::regex_constants::icase);
	    documentation->RegisterEnumValue("", "FlagType", "nosubs", std::regex_constants::nosubs);
	    documentation->RegisterEnumValue("", "FlagType", "optimize", std::regex_constants::optimize);
	    documentation->RegisterEnumValue("", "FlagType", "collate", std::regex_constants::collate);
	    documentation->RegisterEnumValue("", "FlagType", "ECMAScript", std::regex_constants::ECMAScript);
	    documentation->RegisterEnumValue("", "FlagType", "basic", std::regex_constants::basic);
	    documentation->RegisterEnumValue("", "FlagType", "extended", std::regex_constants::extended);
	    documentation->RegisterEnumValue("", "FlagType", "awk", std::regex_constants::awk);
	    documentation->RegisterEnumValue("", "FlagType", "grep", std::regex_constants::grep);
	    documentation->RegisterEnumValue("", "FlagType", "egrep", std::regex_constants::egrep);
	    documentation->RegisterObjectType("Represents a regular expression", "Regex", sizeof(CASRegex), asOBJ_REF);
	    documentation->RegisterObjectBehaviour(
		"Default Constructor",
		"Regex",
		asBEHAVE_FACTORY,
		"Regex@ Regex()",
		asFUNCTION(DefaultConstructRegex),
		asCALL_CDECL);
	    documentation->RegisterObjectBehaviour(
		"Copy Constructor",
		"Regex",
		asBEHAVE_FACTORY,
		"Regex@ Regex(const Regex@ pOther)",
		asFUNCTION(CopyConstructRegex),
		asCALL_CDECL);
	    documentation->RegisterObjectBehaviour(
		"Initialization Constructor",
		"Regex",
		asBEHAVE_FACTORY,
		"Regex@ Regex(const string& in szExpression, FlagType flags = Regex::ECMAScript)",
		asFUNCTION(InitializeConstructRegex),
		asCALL_CDECL);
	    documentation->RegisterObjectBehaviour("AddRef", "Regex", asBEHAVE_ADDREF, "void AddRef()", asMETHOD(CASRegex, AddRef), asCALL_THISCALL);
	    documentation->RegisterObjectBehaviour("Release", "Regex", asBEHAVE_RELEASE, "void Release()", asMETHOD(CASRegex, Release), asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns the given regular expression to this one.",
		"Regex",
		"Regex& Assign(const Regex@ pOther)",
		asMETHODPR(CASRegex, Assign, (const CASRegex *), CASRegex &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns the given regular expression to this one.",
		"Regex",
		"Regex& Assign(const string& in szExpression, FlagType flags = Regex::ECMAScript)",
		asMETHODPR(CASRegex, Assign, (const CString &, int), CASRegex &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the mark count",
		"Regex",
		"uint GetMarkCount() const",
		asMETHOD(CASRegex, GetMarkCount),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets this regex's flags",
		"Regex",
		"FlagType GetFlags() const",
		asMETHOD(CASRegex, GetFlags),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Swaps this regular expression's contents with the given expression",
		"Regex",
		"void Swap(Regex@ other)",
		asMETHOD(CASRegex, Swap),
		asCALL_THISCALL);
    }

    static void RegisterSCScriptRegexMatchResults(CASDocumentation *documentation)
    {
	    documentation->RegisterEnum("Match format flags. These can be bitwise OR'd together to combine options.", "MatchFlagType");
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_default", std::regex_constants::match_default);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_not_bol", std::regex_constants::match_not_bol);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_not_eol", std::regex_constants::match_not_eol);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_not_bow", std::regex_constants::match_not_bow);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_not_eow", std::regex_constants::match_not_eow);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_any", std::regex_constants::match_any);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_not_null", std::regex_constants::match_not_null);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_continuous", std::regex_constants::match_continuous);
	    documentation->RegisterEnumValue("", "MatchFlagType", "match_prev_avail", std::regex_constants::match_prev_avail);
	    documentation->RegisterEnumValue("", "MatchFlagType", "format_default", std::regex_constants::format_default);
	    documentation->RegisterEnumValue("", "MatchFlagType", "format_sed", std::regex_constants::format_sed);
	    documentation->RegisterEnumValue("", "MatchFlagType", "format_no_copy", std::regex_constants::format_no_copy);
	    documentation->RegisterEnumValue("", "MatchFlagType", "format_first_only", std::regex_constants::format_first_only);
	    documentation->RegisterObjectType("Regular expression match results", "MatchResults", 0, asOBJ_REF);
	    documentation->RegisterObjectBehaviour(
		"Constructor",
		"MatchResults",
		asBEHAVE_FACTORY,
		"MatchResults@ MatchResults()",
		asFUNCTION(ConstructRegexMatchResults),
		asCALL_CDECL);
	    documentation->RegisterObjectBehaviour(
		"Copy constructor",
		"MatchResults",
		asBEHAVE_FACTORY,
		"MatchResults@ MatchResults(const MatchResults@ pOther)",
		asFUNCTION(CopyConstructRegexMatchResults),
		asCALL_CDECL);
	    documentation->RegisterObjectBehaviour(
		"AddRef",
		"MatchResults",
		asBEHAVE_ADDREF,
		"void AddRef()",
		asMETHOD(CASRegexMatchResults, AddRef),
		asCALL_THISCALL);
	    documentation->RegisterObjectBehaviour(
		"Release",
		"MatchResults",
		asBEHAVE_RELEASE,
		"void Release()",
		asMETHOD(CASRegexMatchResults, Release),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns whether this match results object is empty or not",
		"MatchResults",
		"bool IsEmpty() const",
		asMETHOD(CASRegexMatchResults, IsEmpty),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns the number of matches",
		"MatchResults",
		"size_t GetSize() const",
		asMETHOD(CASRegexMatchResults, GetSize),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns the maximum size",
		"MatchResults",
		"size_t GetMaxSize() const",
		asMETHOD(CASRegexMatchResults, GetMaxSize),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns the sub match object at the given index",
		"MatchResults",
		"SubMatch opIndex(size_t uiIndex) const",
		asMETHOD(CASRegexMatchResults, operator[]),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the prefix submatch",
		"MatchResults",
		"SubMatch GetPrefix() const",
		asMETHOD(CASRegexMatchResults, GetPrefix),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the suffix submatch",
		"MatchResults",
		"SubMatch GetSuffix() const",
		asMETHOD(CASRegexMatchResults, GetSuffix),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the length of the given submatch",
		"MatchResults",
		"int GetLength(size_t uiIndex = 0) const",
		asMETHOD(CASRegexMatchResults, GetLength),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the position of the given submatch",
		"MatchResults",
		"int GetPosition(size_t uiIndex = 0) const",
		asMETHOD(CASRegexMatchResults, GetPosition),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the given submatch as a string",
		"MatchResults",
		"string GetString(size_t uiIndex = 0) const",
		asMETHOD(CASRegexMatchResults, GetString),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the length of the given submatch",
		"MatchResults",
		"string Format(const string& in szFormat, MatchFlagType flags = Regex::match_default) const",
		asMETHOD(CASRegexMatchResults, Format),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Swaps these match results with the given results.",
		"MatchResults",
		"void Swap(MatchResults@ other)",
		asMETHOD(CASRegexMatchResults, Swap),
		asCALL_THISCALL);
    }

    void RegisterSCScriptRegex(CASDocumentation *documentation)
    {
	    documentation->SetDefaultNamespace("Regex");
	    RegisterSCScriptRegexConstants(documentation);
	    documentation->RegisterObjectType("Regular expression submatch", "SubMatch", 4, asOBJ_VALUE | asOBJ_APP_CLASS_DK);
	    documentation->RegisterObjectBehaviour(
		"Constructor. Do not use.",
		"SubMatch",
		asBEHAVE_CONSTRUCT,
		"void SubMatch()",
		asFUNCTION(DefaultConstructRegexSubMatch),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Copy constructor",
		"SubMatch",
		asBEHAVE_CONSTRUCT,
		"void SubMatch(const SubMatch& in other)",
		asFUNCTION(CopyConstructRegexSubMatch),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Destructor",
		"SubMatch",
		asBEHAVE_DESTRUCT,
		"void DestructSubMatch()",
		asFUNCTION(DestructRegexSubMatch),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectMethod(
		"Gets the length of the submatch",
		"SubMatch",
		"int GetLength() const",
		asMETHOD(CASRegexSubMatch, GetLength),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets this submatch as a string",
		"SubMatch",
		"string GetString() const",
		asMETHOD(CASRegexSubMatch, GetString),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares this submatch to another",
		"SubMatch",
		"int Compare(const SubMatch& in other) const",
		asMETHODPR(CASRegexSubMatch, Compare, (const CASRegexSubMatch &) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares this submatch to a string",
		"SubMatch",
		"int Compare(const string& in szString) const",
		asMETHODPR(CASRegexSubMatch, Compare, (const CString &) const, int),
		asCALL_THISCALL);
	    RegisterSCScriptRegexMatchResults(documentation);
	    documentation->RegisterGlobalFunction(
		"Performs are regular expression matching operation.\nReturns true if the match succeeded, false otherwise",
		"bool Match(const string& in szString, const Regex@ pRegex, MatchFlagType flags = match_default)",
		asFUNCTIONPR(ASRegexMatch, (const CString &, const CASRegex *, std::regex_constants::match_flag_type), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Performs are regular expression matching operation.\nReturns true if the match succeeded, false "
		"otherwise.\npResults will contain the results of the operation, if any.",
		"bool Match(const string& in szString, MatchResults@ pResults, const Regex@ pRegex, MatchFlagType flags = "
		"match_default)",
		asFUNCTIONPR(ASRegexMatch, (const CString &, CASRegexMatchResults *, const CASRegex *, std::regex_constants::match_flag_type), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether some sub-sequence in the target sequence (the subject) matches the regular expression pRegex.",
		"bool Search(const string& in szString, const Regex@ pRegex, MatchFlagType flags = match_default)",
		asFUNCTIONPR(ASRegexSearch, (const CString &, const CASRegex *, std::regex_constants::match_flag_type), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether some sub-sequence in the target sequence (the subject) matches the regular expression "
		"pRegex.\npResults will contain the results of the operation, if any.",
		"bool Search(const string& in szString, MatchResults@ pResults, const Regex@ pRegex, MatchFlagType flags = "
		"match_default)",
		asFUNCTIONPR(ASRegexSearch, (const CString &, CASRegexMatchResults *, const CASRegex *, std::regex_constants::match_flag_type), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Makes a copy of the target sequence (the subject) with all matches of the regular expression pRegex (the "
		"pattern) replaced by szFormat (the replacement).",
		"string Replace(const string& in szString, const Regex@ pRegex, const string& in szFormat, MatchFlagType flags "
		"= match_default)",
		asFUNCTION(ASRegexReplace),
		asCALL_CDECL);
	    documentation->SetDefaultNamespace("");
    }
