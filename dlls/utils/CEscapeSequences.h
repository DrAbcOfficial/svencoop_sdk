#ifndef CESCAPESEQUENCES_H
#define CESCAPESEQUENCES_H

class CEscapeSequences
{
public:
	enum
	{
		CHARACTER_COUNT = 256,
		NO_ESCAPE_CHARACTER = 127
	};

	struct ConversionData_t
	{
		char		m_cCharacter;
		const char	*m_pszString;
	};

	CEscapeSequences(char escapeCharacter, unsigned int count, const ConversionData_t *conversions);

	const char *GetString(char character) const;
	unsigned int GetStringLength(char character) const;
	char GetEscapeSequence(const char *text) const;
	char GetEscapeCharacter(void) const { return m_cEscapeCharacter; }

private:
	struct StringData_t
	{
		const char		*m_pszString;
		unsigned int	m_uiLength;
	};

	char			m_cEscapeCharacter;
	unsigned int	m_uiCount;
	StringData_t	m_Strings[CHARACTER_COUNT];
	char			m_Characters[CHARACTER_COUNT];
};

CEscapeSequences &GetEscapeSeqConversion(void);
CEscapeSequences &getNoEscapeSeqConversion(void);

#endif
