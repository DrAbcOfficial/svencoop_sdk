// AngelScript-engine-level test for the migrated string API.
//
// Registers the real script string bindings (ASString.cpp + string factory)
// on a live AngelScript engine and runs script functions that exercise the
// migrated behavior: numeric formatting, the Tokenize cursor contract, the
// find-family quirks, formatting helpers and the global conversion helpers.
// Split() is not exercised because it resolves array<string> through the
// server manager singleton; its token logic is covered by the Tokenize cases
// and the golden string test.
//
// Every case compares the returned string byte for byte against the former
// CString behavior.
#include "mathlib.h"
#include "eiface.h"

#include <angelscript.h>

#include "scripting/api/ASString.h"
#include "scripting/core/CASDocumentation.h"
#include "../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

#include <cstdio>
#include <cstring>
#include <string>

namespace
{
	int g_Failures = 0;
	bool g_ScriptError = false;

	void ASMessageCallback(const asSMessageInfo *message, void *)
	{
		if (message->type == asMSGTYPE_ERROR)
		{
			std::fprintf(stderr, "script error: %s(%d:%d): %s\n",
				message->section, message->row, message->col, message->message);
			g_ScriptError = true;
		}
	}

	void Check(asIScriptEngine *engine, asIScriptModule *module,
		const char *function, const std::string &expected)
	{
		asIScriptContext *context = engine->CreateContext();
		asIScriptFunction *func = module->GetFunctionByName(function);

		if (!func)
		{
			std::printf("FAIL %s: function missing\n", function);
			g_Failures++;
			return;
		}

		g_ScriptError = false;
		int result = context->Prepare(func);

		if (result >= 0)
			result = context->Execute();

		if (result != asEXECUTION_FINISHED || g_ScriptError)
		{
			std::printf("FAIL %s: execution did not finish (result %d)\n", function, result);
			g_Failures++;
			context->Release();
			return;
		}

		std::string actual = *(std::string *) context->GetReturnAddress();
		context->Release();

		if (actual != expected)
		{
			std::printf("FAIL %s:\n  expected [%s]\n  actual   [%s]\n",
				function, expected.c_str(), actual.c_str());
			g_Failures++;
			return;
		}

		std::printf("PASS %s\n", function);
	}

	const char *g_ScriptSource = R"SCRIPT(
string test_constructors()
{
	return string(3.14) + "|" + string(2.0) + "|" + string(0.5) + "|" + string(true) + "|" +
	       string(false) + "|" + string(42) + "|" + string(-7) + "|" + string(2147483647) + "|" +
	       string('A') + "|" + string(-9223372036854775807LL - 1);
}

string test_numeric_assign_and_append()
{
	string s;
	s = 2.0;          // "2"
	s += 1.25;        // "21.25"
	s += 2.0;         // "212"  (trailing zeros trimmed)
	s += -13;         // "212-13"
	s += true;        // "212-131"
	s += 'Z';         // "212-131Z"
	s = -9223372036854775807LL - 1;
	return s;         // "-9223372036854775808"
}

string test_length_empty_clear()
{
	string s = "hello";
	string out = "" + s.Length() + "|" + s.IsEmpty() + "|";
	s.Clear();
	out += s.Length() + "|" + s.IsEmpty();
	return out;       // "5|false|0|true"
}

string test_tokenize_sequence()
{
	string s = "a,b,c";
	string out = s.Tokenize(",");
	out += "," + s.Tokenize(",");
	out += "," + s.Tokenize(",");
	out += "," + s.Tokenize(",");   // NO_MORE_TOKENS as numeric string
	out += "," + s.Tokenize(",");   // cursor wrapped back to the start
	return out;                     // "a,b,c,4294967294,a"
}

string test_tokenize_empty_middles()
{
	string s = "one,,three";
	string out = s.Tokenize(",");
	out += "|" + s.Tokenize(",");
	out += "|" + s.Tokenize(",");
	out += "|" + s.Tokenize(",");
	return out;                     // "one||three|4294967294"
}

string test_tokenize_reset_by_assign()
{
	string s = "x,y";
	string t = s.Tokenize(",");   // "x"
	s = "reset,z";                // assignment resets the cursor
	return t + "," + s.Tokenize(",");   // "x,reset"
}

string test_find_family()
{
	string s = "Hello, World!";
	string r = "" + s.Find("o");
	r += "," + s.Find("o", 5);
	r += "," + s.Find("Z");
	r += "," + s.RFind("o");
	r += "," + s.FindFirstOf("ol");
	r += "," + s.FindLastOf("ol");
	r += "," + s.Find("world", 0, String::CaseInsensitive);
	r += "," + s.Find("world");
	return r;   // "4,8,4294967295,8,4,10,7,4294967295"
}

string test_findfirstof_case_insensitive_quirk()
{
	// CaseInsensitive scans the character set from its second character.
	string s = "Hello, World!";
	string r = "" + s.FindFirstOf("OL", 0, String::CaseInsensitive);   // matches 'l' via set[1]
	r += "," + s.FindFirstOf("OL");                                    // exact case: neither present
	return r;   // "2,4294967295"
}

string test_findfirstnotof_case_insensitive_quirk()
{
	// CaseInsensitive membership always reports "not of" (set scan skips the
	// entire set), so the search stops immediately at startIndex.
	string s = "Hello";
	return "" + s.FindFirstNotOf("elH", 0, String::CaseInsensitive) + "," +
	       s.FindFirstNotOf("elH");   // "0,3"
}

string test_findlastnotof_ignores_compare_type()
{
	string s = "aXa";
	return "" + s.FindLastNotOf("a", String::INVALID_INDEX, String::CaseInsensitive) + "," +
	       s.FindLastNotOf("a", String::INVALID_INDEX, String::CaseInsensitive);   // "1,1"
}

string test_replace()
{
	string a = "aAaA";
	a.Replace("aa", "-");                                   // case-sensitive: no match
	string b = "aAaA";
	b.Replace("aa", "-", String::CaseInsensitive);          // "--"
	string c = "One Two One";
	c.Replace("One", "1");                                  // "1 Two 1"
	return a + "|" + b + "|" + c;   // "aAaA|--|1 Two 1"
}

string test_substring_and_index()
{
	string s = "hello";
	string r = s.SubString(1, 3) + "|" + s.SubString(99) + "|" + s[99] + "|" + s[1];
	return r;   // "ell|o|h|e"  (out-of-range opIndex returns the first character)
}

string test_trim_lower_upper()
{
	string s = "  padded  ";
	s.Trim();
	string mixed = "MiXeD";
	return s + "|" + mixed.ToLowercase() + "|" + mixed.ToUppercase() + "|" + mixed;
	// "padded|mixed|MIXED|MIXED"  (case conversion mutates in place)
}

string test_starts_ends()
{
	string s = "HelloWorld";
	return "" + s.StartsWith("Hello") + "," + s.EndsWith("world", String::CaseInsensitive) + "," +
	       s.EndsWith("WORLD", String::CaseInsensitive) + "," + s.StartsWith("X");   // "true,false,true,false"
}

string test_compare()
{
	string s = "abc";
	return "" + s.Compare("abc") + "," + (s.Compare("abd") < 0 ? 1 : 0) + "," +
	       s.ICompare("ABC") + "," + s.opEquals("abc") + "," + (s.opCmp("abd") < 0 ? 1 : 0);
	// "0,1,0,true,true" (ICompare raw strcasecmp sign on glibc is 0 for equal)
}

string test_format_helpers()
{
	return formatFloat(3.14159, "", 0, 2) + "," + formatInt(255, "h") + "," +
	       formatUInt(255, "H") + "," + formatFloat(-0.5, "", 0, 1);
	// "3.14,ff,FF,-0.5"
}

string test_conversions()
{
	string out = "" + atoi("42") + "," + atoui("ff", 16) + "," + atobool("true") + "," +
	       tolower("AbC") + "," + toupper("AbC") + "," + atof("2.5");
	// "42,255,true,abc,ABC,2.5"
	return out;
}

string test_snprintf()
{
	string out;
	bool ok = snprintf(out, "%s=%d!", "key", 5);
	return "" + ok + "," + out;   // "true,key=5!"
}

string test_global_sentinels()
{
	return "" + String::INVALID_INDEX + "," + String::NO_MORE_TOKENS;
	// "4294967295,4294967294"
}

string test_split()
{
	array<string>@ parts = "a,b,c".Split(",");
	string out = "" + parts.length() + ":" + parts[0] + "," + parts[1] + "," + parts[2];
	array<string>@ emptyDelim = "keep".Split("");
	return out + "|" + emptyDelim.length() + "|" + emptyDelim[0];   // "3:a,b,c|1|keep"
}

string test_default_and_empty()
{
	string s;
	return "" + s.Length() + "|" + s.IsEmpty() + "|" + (s == "") + "|" + (s == String::EMPTY_STRING);
	// "0|true|true|true"
}
)SCRIPT";
}

int main()
{
	asIScriptEngine *engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);

	if (!engine)
	{
		std::fprintf(stderr, "failed to create AngelScript engine\n");
		return 2;
	}

	engine->SetMessageCallback(asFUNCTION(ASMessageCallback), NULL, asCALL_CDECL);

	RegisterScriptArray(engine, false);

	CASDocumentation *documentation = new CASDocumentation(engine);
	RegisterSCScriptASString(engine, documentation);

	asIScriptModule *module = engine->GetModule("asstring_test", asGM_ALWAYS_CREATE);
	module->AddScriptSection("asstring_test", g_ScriptSource, std::strlen(g_ScriptSource));

	if (module->Build() < 0)
	{
		std::fprintf(stderr, "module build failed\n");
		return 2;
	}

	Check(engine, module, "test_constructors",
		"3.14|2|0.5|1|0|42|-7|2147483647|A|-9223372036854775808");
	Check(engine, module, "test_numeric_assign_and_append", "-9223372036854775808");
	Check(engine, module, "test_length_empty_clear", "5|false|0|true");
	Check(engine, module, "test_tokenize_sequence", "a,b,c,4294967294,a");
	Check(engine, module, "test_tokenize_empty_middles", "one||three|4294967294");
	Check(engine, module, "test_tokenize_reset_by_assign", "x,reset");
	Check(engine, module, "test_find_family", "4,8,4294967295,8,4,10,7,4294967295");
	Check(engine, module, "test_findfirstof_case_insensitive_quirk", "2,4294967295");
	Check(engine, module, "test_findfirstnotof_case_insensitive_quirk", "0,3");
	Check(engine, module, "test_findlastnotof_ignores_compare_type", "1,1");
	Check(engine, module, "test_replace", "aAaA|--|1 Two 1");
	Check(engine, module, "test_substring_and_index", "ell|o|h|e");
	Check(engine, module, "test_trim_lower_upper", "padded|mixed|MIXED|MIXED");
	Check(engine, module, "test_starts_ends", "true,false,true,false");
	Check(engine, module, "test_compare", "0,1,0,true,true");
	Check(engine, module, "test_format_helpers", "3.14,ff,FF,-0.5");
	Check(engine, module, "test_conversions", "42,255,true,abc,ABC,2.5");
	Check(engine, module, "test_snprintf", "true,key=5!");
	Check(engine, module, "test_split", "3:a,b,c|1|keep");
	Check(engine, module, "test_global_sentinels", "4294967295,4294967294");
	Check(engine, module, "test_default_and_empty", "0|true|true|true");

	delete documentation;
	engine->ShutDownAndRelease();

	if (g_Failures)
	{
		std::printf("FAILED: %d case(s)\n", g_Failures);
		return 1;
	}

	std::printf("OK: all script string cases pass\n");
	return 0;
}
