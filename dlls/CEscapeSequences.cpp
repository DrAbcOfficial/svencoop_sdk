#include "CEscapeSequences.h"
#include <string.h>

static const CEscapeSequences::ConversionData_t g_EscapeSequencesList[] =
{
	{ '"', "\\\"" },
	{ '\'', "\\'" },
	{ '\\', "\\\\" },
	{ '\t', "\\t" },
	{ '\n', "\\n" },
	{ '\a', "\\a" },
	{ '\b', "\\b" },
	{ '\f', "\\f" },
	{ '\v', "\\v" },
	{ '\0', "\\0" }
};

static const CEscapeSequences::ConversionData_t g_NoEscapeSequencesList[] =
{
	{ CEscapeSequences::NO_ESCAPE_CHARACTER, "" }
};

static CEscapeSequences EscapeSequences('\\',
	sizeof(g_EscapeSequencesList) / sizeof(g_EscapeSequencesList[0]), g_EscapeSequencesList);
static CEscapeSequences NoEscapeSequences(CEscapeSequences::NO_ESCAPE_CHARACTER,
	sizeof(g_NoEscapeSequencesList) / sizeof(g_NoEscapeSequencesList[0]), g_NoEscapeSequencesList);

CEscapeSequences::CEscapeSequences(char escapeCharacter, unsigned int count,
	const ConversionData_t *conversions)
	: m_cEscapeCharacter(escapeCharacter), m_uiCount(count)
{
	memset(m_Strings, 0, sizeof(m_Strings));
	memset(m_Characters, 0, sizeof(m_Characters));

	for (unsigned int i = 0; i < m_uiCount; i++)
	{
		char character = conversions[i].m_cCharacter;

		m_Characters[i] = character;
		m_Strings[character].m_pszString = conversions[i].m_pszString;
		m_Strings[character].m_uiLength = strlen(conversions[i].m_pszString);
	}
}

const char *CEscapeSequences::GetString(char character) const
{
	return m_Strings[character].m_pszString;
}

unsigned int CEscapeSequences::GetStringLength(char character) const
{
	return m_Strings[character].m_uiLength;
}

char CEscapeSequences::GetEscapeSequence(const char *text) const
{
	for (unsigned int i = 0; i < m_uiCount; i++)
	{
		char character = m_Characters[i];
		const char *conversion = m_Strings[character].m_pszString;

		if (conversion && !strcmp(conversion, text))
			return character;
	}

	return 0;
}

CEscapeSequences &GetEscapeSeqConversion(void)
{
	return EscapeSequences;
}

CEscapeSequences &getNoEscapeSeqConversion(void)
{
	return NoEscapeSequences;
}
