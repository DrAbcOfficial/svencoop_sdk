#include "extdll.h"
#include "scripting/api/ASString.h"
#include "scripting/core/CASDocumentation.h"
#include "CString.h"
#include <ctype.h>
#include <stdlib.h>
#include <new>
#include "scripting/core/CCStringFactory.h"
#include "scripting/core/CASServerManager.h"
#include "../../../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

namespace
{
	void DefaultConstructChar(void *memory)
	{
		*(char *)memory = 0;
	}

	void CopyConstructChar(const char &character, void *memory)
	{
		*(char *)memory = character;
	}

	void CopyConstructChar(const CString &text, void *memory)
	{
		*(char *)memory = text.Length() ? text.CharAt(0) : 0;
	}

	void DestructChar(void *memory)
	{
	}

	char Char_Assign(const char &character, char *destination)
	{
		*destination = character;
		return character;
	}

	unsigned int Char_ToUint32(const char *character)
	{
		return (signed char)*character;
	}

	char Char_Assign_String(const CString &text, char *destination)
	{
		if (!text.Length())
		{
			*destination = 0;
			// The empty branch returns the destination address truncated to a char
			return (char)(size_t)destination;
		}

		*destination = text.CharAt(0);
		return *destination;
	}

	bool Char_Compare_String(const CString &text, const char *character)
	{
		return *character == text[0];
	}

	bool isalnum(char character)
	{
		return ::isalnum(character) != 0;
	}

	bool isalpha(char character)
	{
		return ::isalpha(character) != 0;
	}

	bool iscntrl(char character)
	{
		return ::iscntrl(character) != 0;
	}

	bool isdigit(char character)
	{
		return ::isdigit(character) != 0;
	}

	bool isgraph(char character)
	{
		return ::isgraph(character) != 0;
	}

	bool islower(char character)
	{
		return ::islower(character) != 0;
	}

	bool isprint(char character)
	{
		return ::isprint(character) != 0;
	}

	bool ispunct(char character)
	{
		return ::ispunct(character) != 0;
	}

	bool isspace(char character)
	{
		return ::isspace(character) != 0;
	}

	bool isupper(char character)
	{
		return ::isupper(character) != 0;
	}

	bool isxdigit(char character)
	{
		return ::isxdigit(character) != 0;
	}

	char tolower(char character)
	{
		return (char)::tolower(character);
	}

	char toupper(char character)
	{
		return (char)::toupper(character);
	}

	void RegisterSCScriptCharFunctions(CASDocumentation *documentation)
	{
		documentation->RegisterGlobalFunction("Returns whether character is alphanumeric.",
			"bool isalnum(char character)", asFUNCTIONPR(isalnum, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is alphabetic.",
			"bool isalpha(char character)", asFUNCTIONPR(isalpha, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is control.",
			"bool iscntrl(char character)", asFUNCTIONPR(iscntrl, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is a decimal digit character.",
			"bool isdigit(char character)", asFUNCTIONPR(isdigit, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is graph.",
			"bool isgraph(char character)", asFUNCTIONPR(isgraph, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is lower.",
			"bool islower(char character)", asFUNCTIONPR(islower, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is print.",
			"bool isprint(char character)", asFUNCTIONPR(isprint, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is punctuality.",
			"bool ispunct(char character)", asFUNCTIONPR(ispunct, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is space.",
			"bool isspace(char character)", asFUNCTIONPR(isspace, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is upper.",
			"bool isupper(char character)", asFUNCTIONPR(isupper, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Returns whether character is a hexadecimal numeric character.",
			"bool isxdigit(char character)", asFUNCTIONPR(isxdigit, (char), bool), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Converts the given character to lower case.",
			"char tolower(char character)", asFUNCTIONPR(tolower, (char), char), asCALL_CDECL);
		documentation->RegisterGlobalFunction("Converts the given character to upper case.",
			"char toupper(char character)", asFUNCTIONPR(toupper, (char), char), asCALL_CDECL);
	}
}

void RegisterSCScriptForwardDeclareString(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("String type", "string", sizeof(CString),
		asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
	documentation->RegisterObjectType("Char value type", "char", sizeof(char),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_PRIMITIVE);
	RegisterSCScriptCharFunctions(documentation);

	documentation->RegisterObjectBehaviour("Default constructor", "char", asBEHAVE_CONSTRUCT,
		"void char()", asFUNCTION(DefaultConstructChar), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructor", "char", asBEHAVE_CONSTRUCT,
		"void char(const char& in character)", asFUNCTIONPR(CopyConstructChar, (const char &, void *), void),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructor", "char", asBEHAVE_CONSTRUCT,
		"void char(const string& in szString)", asFUNCTIONPR(CopyConstructChar, (const CString &, void *), void),
		asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Destructor", "char", asBEHAVE_DESTRUCT,
		"void char()", asFUNCTION(DestructChar), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assigns one character to another.", "char",
		"char opAssign(const char& in character)", asFUNCTION(Char_Assign), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Implicitly converts this character to an unsigned 32 bit integer.", "char",
		"uint32 opImplConv() const", asFUNCTION(Char_ToUint32), asCALL_CDECL_OBJFIRST);
	documentation->RegisterObjectMethod("Assigns the first character of the given string to this character.", "char",
		"char opAssign(const string& in szString)", asFUNCTION(Char_Assign_String), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Compares this char with the given character.", "char",
		"bool opEquals(const string& in szString) const", asFUNCTION(Char_Compare_String), asCALL_CDECL_OBJLAST);
}


namespace
{
	CCStringFactory g_StringFactory;

	void ConstructASString(void *memory)
	{
		new(memory) CString();
	}

	void CopyConstructASString(const CString &other, void *memory)
	{
	    new (memory) CString(other);
    }

    void ConstructASString(double value, void *memory)
    {
	    new (memory) CString(value);
    }

    void ConstructASString(long long value, void *memory)
    {
	    new (memory) CString(value);
    }

    void ConstructASString(unsigned long long value, void *memory)
    {
	    new (memory) CString(value);
    }

    void ConstructASString(bool value, void *memory)
    {
	    new (memory) CString(value);
    }

    void ConstructASString(char value, void *memory)
    {
	    new (memory) CString(value);
    }

    void DestructASString(CString *text)
    {
	    text->~CString();
    }

    float ToFloat(const CString &text)
    {
	    return (float)strtod(text.Data(), NULL);
    }

    double ToDouble(const CString &text)
    {
	    return strtod(text.Data(), NULL);
    }

    int ToInt(const CString &text, int radix)
    {
	    return strtol(text.Data(), NULL, radix);
    }

    long long ToInt64(const CString &text, int radix)
    {
	    return strtoll(text.Data(), NULL, radix);
    }

    unsigned int ToUnsignedInt(const CString &text, int radix)
    {
	    return strtoul(text.Data(), NULL, radix);
    }

    unsigned long long ToUnsignedInt64(const CString &text, int radix)
    {
	    return strtoull(text.Data(), NULL, radix);
    }

    bool ToBool(const CString &text, bool &valid)
    {
	    if (!stricmp(text.Data(), "true"))
	    {
		    valid = true;
		    return true;
	    }

	    valid = !stricmp(text.Data(), "false");
	    return false;
    }

    bool ToBool(const CString &text)
    {
	    bool valid;

	    return ToBool(text, valid);
    }

    bool isalnum(const CString &text)
    {
	    return ::isalnum((unsigned char)text[0]) != 0;
    }

    bool isalpha(const CString &text)
    {
	    return ::isalpha((unsigned char)text[0]) != 0;
    }

    bool iscntrl(const CString &text)
    {
	    return ::iscntrl((unsigned char)text[0]) != 0;
    }

    bool isdigit(const CString &text)
    {
	    return ::isdigit((unsigned char)text[0]) != 0;
    }

    bool isgraph(const CString &text)
    {
	    return ::isgraph((unsigned char)text[0]) != 0;
    }

    bool islower(const CString &text)
    {
	    return ::islower((unsigned char)text[0]) != 0;
    }

    bool isprint(const CString &text)
    {
	    return ::isprint((unsigned char)text[0]) != 0;
    }

    bool ispunct(const CString &text)
    {
	    return ::ispunct((unsigned char)text[0]) != 0;
    }

    bool isspace(const CString &text)
    {
	    return ::isspace((unsigned char)text[0]) != 0;
    }

    bool isupper(const CString &text)
    {
	    return ::isupper((unsigned char)text[0]) != 0;
    }

    bool isxdigit(const CString &text)
    {
	    return ::isxdigit((unsigned char)text[0]) != 0;
    }

    CString tolower(const CString &text)
    {
	    return CString((char)::tolower((unsigned char)text[0]));
    }

    CString toupper(const CString &text)
    {
	    return CString((char)::toupper((unsigned char)text[0]));
    }

    void ASString_Trim(CString &text, const CString &character)
    {
	    if (character.Length())
		    text.Trim((unsigned char)character[0]);
    }

    void ASSPrintf_CString(asIScriptGeneric *generic)
    {
	    CString *destination = (CString *)generic->GetArgAddress(0);
	    const CString *format = (const CString *)generic->GetArgAddress(1);
	    char buffer[4096];
	    bool success = ASSPrintf(buffer, sizeof(buffer), format->Data(), 2, generic);

	    *destination = buffer;
	    generic->SetReturnByte(success);
    }

    CString formatInt(long long value, const CString &options, unsigned int width)
    {
	    bool left = options.Find('l') != CString::INVALID_INDEX;
	    bool zero = options.Find('0') != CString::INVALID_INDEX;
	    bool sign = options.Find('+') != CString::INVALID_INDEX;
	    bool space = options.Find(' ') != CString::INVALID_INDEX;
	    bool lower = options.Find('h') != CString::INVALID_INDEX;
	    bool upper = options.Find('H') != CString::INVALID_INDEX;
	    CString format("%");

	    if (left)
		    format += "-";

	    if (sign)
		    format += "+";

	    if (space)
		    format += " ";

	    if (zero)
		    format += "0";

	    format += "*ll";
	    format += lower ? "x" : upper ? "X"
	                                  : "d";

	    CString result;

	    result.Format(format.Data(), width, value);
	    return result;
    }

    CString formatUInt(unsigned long long value, const CString &options, unsigned int width)
    {
	    bool left = options.Find('l') != CString::INVALID_INDEX;
	    bool zero = options.Find('0') != CString::INVALID_INDEX;
	    bool sign = options.Find('+') != CString::INVALID_INDEX;
	    bool space = options.Find(' ') != CString::INVALID_INDEX;
	    bool lower = options.Find('h') != CString::INVALID_INDEX;
	    bool upper = options.Find('H') != CString::INVALID_INDEX;
	    CString format("%");

	    if (left)
		    format += "-";

	    if (sign)
		    format += "+";

	    if (space)
		    format += " ";

	    if (zero)
		    format += "0";

	    format += "*ll";
	    format += lower ? "x" : upper ? "X"
	                                  : "u";

	    CString result;

	    result.Format(format.Data(), width, value);
	    return result;
    }

    CString formatFloat(double value, const CString &options, unsigned int width, unsigned int precision)
    {
	    bool left = options.Find('l') != CString::INVALID_INDEX;
	    bool zero = options.Find('0') != CString::INVALID_INDEX;
	    bool sign = options.Find('+') != CString::INVALID_INDEX;
	    bool space = options.Find(' ') != CString::INVALID_INDEX;
	    bool lower = options.Find('e') != CString::INVALID_INDEX;
	    bool upper = options.Find('E') != CString::INVALID_INDEX;
	    CString format("%");

	    if (left)
		    format += "-";

	    if (sign)
		    format += "+";

	    if (space)
		    format += " ";

	    if (zero)
		    format += "0";

	    format += "*.*";
	    format += lower ? "e" : upper ? "E"
	                                  : "f";

	    CString result;

	    result.Format(format.Data(), width, precision, value);
	    return result;
    }
    }

    CScriptArray *StringSplit(const CString &delimiter, const CString &text)
    {
	    asIScriptEngine *engine = CASServerManager::GetInstance()->m_pEngine;
	    asITypeInfo *type = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	    CScriptArray *array = CScriptArray::Create(type);
	    unsigned int index = 0;
	    unsigned int start = 0;
	    unsigned int end;

	    while ((end = text.Find(delimiter, start)) != CString::INVALID_INDEX)
	    {
		    array->Resize(array->GetSize() + 1);
		    ((CString *)array->At(index))->Assign(&text[start], 0, end - start);
		    index++;
		    start = end + delimiter.Length();
	    }

	    array->Resize(array->GetSize() + 1);
	    *(CString *)array->At(index) = &text[start];
	    return array;
    }

    static void RegisterSCScriptStringFunctions(CASDocumentation *documentation)
    {
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a float.",
		"float atof(const string& in)",
		asFUNCTION(ToFloat),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a double.",
		"double atod(const string& in)",
		asFUNCTION(ToDouble),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to an integer.",
		"int atoi(const string& in, int radix = 10)",
		asFUNCTION(ToInt),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a 64 bit integer.",
		"int64 atoi64(const string& in, int radix = 10)",
		asFUNCTION(ToInt64),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to an unsigned integer.",
		"uint atoui(const string& in, int radix = 10)",
		asFUNCTION(ToUnsignedInt),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a 64 bit unsigned integer.",
		"uint64 atoui64(const string& in, int radix = 10)",
		asFUNCTION(ToUnsignedInt64),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a boolean.",
		"bool atobool(const string& in)",
		asFUNCTIONPR(ToBool, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given string to a boolean. fIsValid is true if the given string was a valid boolean value.",
		"bool atobool(const string& in, bool& out fIsValid)",
		asFUNCTIONPR(ToBool, (const CString &, bool &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is alphanumeric.",
		"bool isalnum(const string& in)",
		asFUNCTIONPR(isalnum, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is alphabetic.",
		"bool isalpha(const string& in)",
		asFUNCTIONPR(isalpha, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is control.",
		"bool iscntrl(const string& in)",
		asFUNCTIONPR(iscntrl, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is digit.",
		"bool isdigit(const string& in)",
		asFUNCTIONPR(isdigit, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is graph.",
		"bool isgraph(const string& in)",
		asFUNCTIONPR(isgraph, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is lower.",
		"bool islower(const string& in)",
		asFUNCTIONPR(islower, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is print.",
		"bool isprint(const string& in)",
		asFUNCTIONPR(isprint, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is punctuality.",
		"bool ispunct(const string& in)",
		asFUNCTIONPR(ispunct, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is space.",
		"bool isspace(const string& in)",
		asFUNCTIONPR(isspace, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is upper.",
		"bool isupper(const string& in)",
		asFUNCTIONPR(isupper, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Returns whether character is xdigit.",
		"bool isxdigit(const string& in)",
		asFUNCTIONPR(isxdigit, (const CString &), bool),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given character to lower case.",
		"string tolower(const string& in)",
		asFUNCTIONPR(tolower, (const CString &), CString),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Converts the given character to upper case.",
		"string toupper(const string& in)",
		asFUNCTIONPR(toupper, (const CString &), CString),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Formats a signed integer into a string.\nThe options string is a combination of the following characters:\n'l' = "
		"left justify\n'0' = pad with zeroes\n'+' = always include the sign, even if positive\n' ' = add a space in case "
		"of positive number\n'h' = hexadecimal integer small letters\n'H' = hexadecimal integer capital letters\n",
		"string formatInt(int64 value, const string &in options = \"\", uint width = 0)",
		asFUNCTION(formatInt),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Formats a unsigned integer into a string.\nThe options string is a combination of the following characters:\n'l' "
		"= left justify\n'0' = pad with zeroes\n'+' = always include the sign, even if positive\n' ' = add a space in case "
		"of positive number\n'h' = hexadecimal integer small letters\n'H' = hexadecimal integer capital letters\n",
		"string formatUInt(uint64 value, const string &in options = \"\", uint width = 0)",
		asFUNCTION(formatUInt),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Formats a double into a string.\nThe options string is a combination of the following characters:\n'l' = left "
		"justify\n'0' = pad with zeroes\n'+' = always include the sign, even if positive\n' ' = add a space in case of "
		"positive number\n'e' = exponent character with small e\n'E' = exponent character with capital E\n",
		"string formatFloat(double value, const string &in options = \"\", uint width = 0, uint precision = 0)",
		asFUNCTION(formatFloat),
		asCALL_CDECL);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& "
		"in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
	    documentation->RegisterGlobalFunction(
		"Prints the format string to the destination buffer. Uses SC printf formatting style.",
		"bool snprintf(string& out szOutBuffer, const string& in szFormat, ?& in, ?& in, ?& in, ?& in, ?& in, ?& in, ?& "
		"in, ?& in)",
		asFUNCTION(ASSPrintf_CString),
		asCALL_GENERIC);
    }

    void RegisterSCScriptASString(asIScriptEngine *engine, CASDocumentation *documentation)
    {
	    RegisterSCScriptStringFunctions(documentation);
	    documentation->SetDefaultNamespace("String");
	    documentation->RegisterEnum("Compare type.", "CompareType");
	    documentation->RegisterEnumValue("", "CompareType", "CaseSensitive", String::CaseSensitive);
	    documentation->RegisterEnumValue("", "CompareType", "CaseInsensitive", String::CaseInsensitive);
	    documentation->RegisterGlobalProperty("Default comparison type.", "const CompareType DEFAULT_COMPARE", (void *)&String::DEFAULT_COMPARE);
	    documentation->RegisterGlobalProperty(
		"Empty string. Useful when a reference to a string is needed.",
		"const string EMPTY_STRING",
		(void *)&CString::EMPTY_STRING);
	    documentation->RegisterGlobalProperty(
		"A string that contains one instance of every whitespace character.",
		"const string WHITESPACE_CHARACTERS",
		(void *)&CString::WHITESPACE_CHARACTERS);
	    documentation->RegisterGlobalProperty("Sentinel value used by string methods.", "const size_t INVALID_INDEX", (void *)&CString::INVALID_INDEX);
	    documentation->RegisterGlobalProperty(
		"Sentinel value used by string tokenization when there are no tokens remaining.",
		"const size_t NO_MORE_TOKENS",
		(void *)&CString::NO_MORE_TOKENS);
	    documentation->SetDefaultNamespace("");
	    engine->RegisterStringFactory("string", &g_StringFactory);

	    documentation->RegisterObjectBehaviour(
		"Constructor",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string()",
		asFUNCTIONPR(ConstructASString, (void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Copy constructor",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(const string& in szString)",
		asFUNCTION(CopyConstructASString),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Construct a string from a double.",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(double flValue)",
		asFUNCTIONPR(ConstructASString, (double, void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Construct a string from a 64 bit integer.",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(int64 iValue)",
		asFUNCTIONPR(ConstructASString, (long long, void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Construct a string from a 64 bit unsigned integer.",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(uint64 uiValue)",
		asFUNCTIONPR(ConstructASString, (unsigned long long, void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Construct a string from a boolean.",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(bool bValue)",
		asFUNCTIONPR(ConstructASString, (bool, void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Construct a string from a char.",
		"string",
		asBEHAVE_CONSTRUCT,
		"void string(char character)",
		asFUNCTIONPR(ConstructASString, (char, void *), void),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectBehaviour(
		"Destructor",
		"string",
		asBEHAVE_DESTRUCT,
		"void Destructstring()",
		asFUNCTION(DestructASString),
		asCALL_CDECL_OBJLAST);
	    documentation->RegisterObjectMethod(
		"Assigns a string to this, copying up to uiCount characters.",
		"string",
		"string& Assign(const string& in szString, uint uiBegin, uint uiCount)",
		asMETHODPR(CString, Assign, (const CString &, unsigned int, unsigned int), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a string to this one.",
		"string",
		"string& opAssign(const string& in szString)",
		asMETHODPR(CString, operator=, (const CString &), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a double to this string.",
		"string",
		"string& opAssign(double flValue)",
		asMETHODPR(CString, operator=, (double), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a 64 bit integer to this string.",
		"string",
		"string& opAssign(int64 iValue)",
		asMETHODPR(CString, operator=, (long long), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a 64 bit unsigned int to this string.",
		"string",
		"string& opAssign(uint64 uiValue)",
		asMETHODPR(CString, operator=, (unsigned long long), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a boolean to this string.",
		"string",
		"string& opAssign(bool bValue)",
		asMETHODPR(CString, operator=, (bool), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Assigns a character to this string.",
		"string",
		"string& opAssign(char character)",
		asMETHODPR(CString, operator=, (char), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets the length of this string.",
		"string",
		"uint Length() const",
		asMETHOD(CString, Length),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns whether this string is empty.",
		"string",
		"bool IsEmpty() const",
		asMETHOD(CString, IsEmpty),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Shrinks this string to uiSize. If bKeepData is true, the original string is kept.",
		"string",
		"void Resize(uint uiSize, bool bKeepData = true)",
		asMETHOD(CString, Resize),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Reserves at least iMinimum characters worth of capacity. If bKeepData is true, the original string is kept.",
		"string",
		"void Reserve(uint iMinimum, bool bKeepData = true)",
		asMETHOD(CString, Reserve),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod("Clears the contents of this string.", "string", "void Clear()", asMETHOD(CString, Clear), asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Gets a character by index.",
		"string",
		"char opIndex(uint uiIndex) const",
		asMETHOD(CString, CharAt),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Sets a character by index.",
		"string",
		"void SetCharAt(uint uiIndex, char character)",
		asMETHOD(CString, SetCharAt),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given string to this one.",
		"string",
		"string& opAddAssign(const string& in szString)",
		asMETHODPR(CString, operator+=, (const CString &), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given double to this string.",
		"string",
		"string& opAddAssign(double flValue)",
		asMETHODPR(CString, operator+=, (double), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given 64 bit integer to this string.",
		"string",
		"string& opAddAssign(int64 iValue)",
		asMETHODPR(CString, operator+=, (long long), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given 64 bit unsigned integer to this string.",
		"string",
		"string& opAddAssign(uint64 uiValue)",
		asMETHODPR(CString, operator+=, (unsigned long long), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given boolean to this string.",
		"string",
		"string& opAddAssign(bool bValue)",
		asMETHODPR(CString, operator+=, (bool), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Appends the given character to this string.",
		"string",
		"string& opAddAssign(char character)",
		asMETHODPR(CString, operator+=, (char), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares strings.",
		"string",
		"int opCmp(const string& in szString) const",
		asMETHODPR(CString, Compare, (const CString &) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares strings.",
		"string",
		"int Compare(const string& in szString) const",
		asMETHODPR(CString, Compare, (const CString &) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares strings up to amount characters.",
		"string",
		"int CompareN(const string& in szString, const uint amount) const",
		asMETHODPR(CString, CompareN, (const CString &, unsigned int) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares strings case-insensitively.",
		"string",
		"int ICompare(const string& in szString) const",
		asMETHODPR(CString, ICompare, (const CString &) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Compares strings up to amount characters case-insensitively.",
		"string",
		"int ICompareN(const string& in szString, const uint amount) const",
		asMETHODPR(CString, ICompareN, (const CString &, unsigned int) const, int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns whether strings are equal.",
		"string",
		"bool opEquals(const string& in szString) const",
		asMETHODPR(CString, operator==, (const CString &) const, bool),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Trims leading and trailing whitespace, or a given character.",
		"string",
		"void Trim(const string& in szCharacter = ' ')",
		asFUNCTION(ASString_Trim),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns true if the string starts with the given string.",
		"string",
		"bool StartsWith(const string& in szString, const String::CompareType compareType = String::DEFAULT_COMPARE) "
		"const",
		asMETHODPR(CString, StartsWith, (const CString &, String::CompareType) const, bool),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns true if the string ends with the given string.",
		"string",
		"bool EndsWith(const string& in szString, const String::CompareType compareType = String::DEFAULT_COMPARE) "
		"const",
		asMETHODPR(CString, EndsWith, (const CString &, String::CompareType) const, bool),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the first instance of the given string.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint Find(const string& in szString, const uint startIndex = 0, const String::CompareType compareType = "
		"String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, Find, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the first instance of the given string, searching in reverse.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint RFind(const string& in szString, uint startIndex = String::INVALID_INDEX, const String::CompareType "
		"compareType = String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, RFind, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the first instance of any character in the given string.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint FindFirstOf(const string& in szString, const uint startIndex = 0, const String::CompareType compareType = "
		"String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, FindFirstOf, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the last instance of any character in the given string.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint FindLastOf(const string& in szString, const uint startIndex = 0, const String::CompareType compareType = "
		"String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, FindLastOf, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the first instance of the given string.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint FindFirstNotOf(const string& in szString, uint startIndex = 0, const String::CompareType compareType = "
		"String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, FindFirstNotOf, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Find the first instance of the given string, searching in reverse.\nReturns String::INVALID_INDEX if not found.",
		"string",
		"uint FindLastNotOf(const string& in szString, uint startIndex = String::INVALID_INDEX, const "
		"String::CompareType compareType = String::DEFAULT_COMPARE) const",
		asMETHODPR(CString, FindLastNotOf, (const CString &, unsigned int, String::CompareType) const, unsigned int),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Get the next token of the given string.\nReturns String::NO_MORE_TOKENS if none remaining.",
		"string",
		"string Tokenize(const string& in delimiter) const",
		asMETHODPR(CString, Tokenize, (const CString &) const, CString),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Converts this string to lower case.",
		"string",
		"string& ToLowercase()",
		asMETHOD(CString, ToLowercase),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Converts this string to upper case.",
		"string",
		"string& ToUppercase()",
		asMETHOD(CString, ToUppercase),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns a substring of this string.",
		"string",
		"string SubString(uint startIndex = 0, uint count = String::INVALID_INDEX) const",
		asMETHOD(CString, SubString),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Replaces the given substring with the given replacement string.",
		"string",
		"string& Replace(const string& in szSubstring, const string& in szReplacement, const String::CompareType "
		"compareType = String::DEFAULT_COMPARE)",
		asMETHODPR(CString, Replace, (const CString &, const CString &, String::CompareType), CString &),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Truncates the string to uiMaxLength characters.",
		"string",
		"void Truncate(const size_t uiMaxLength)",
		asMETHOD(CString, Truncate),
		asCALL_THISCALL);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given string appended.",
		"string",
		"string opAdd(const string& in szString) const",
		asFUNCTIONPR(operator+, (const CString &, const CString &), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given double appended.",
		"string",
		"string opAdd(double flValue) const",
		asFUNCTIONPR(operator+, (const CString &, double), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given 64 bit integer appended.",
		"string",
		"string opAdd(int64 iValue) const",
		asFUNCTIONPR(operator+, (const CString &, long long), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given 64 bit unsigned integer appended.",
		"string",
		"string opAdd(uint64 uiValue) const",
		asFUNCTIONPR(operator+, (const CString &, unsigned long long), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given boolean appended.",
		"string",
		"string opAdd(bool bValue) const",
		asFUNCTIONPR(operator+, (const CString &, bool), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Returns a copy of this string with the given character appended.",
		"string",
		"string opAdd(char character) const",
		asFUNCTIONPR(operator+, (const CString &, char), CString),
		asCALL_CDECL_OBJFIRST);
	    documentation->RegisterObjectMethod(
		"Splits this string by the given delimiter.",
		"string",
		"array<string>@ Split(const string& in szDelimiter) const",
		asFUNCTION(StringSplit),
		asCALL_CDECL_OBJLAST);
    }
