#ifndef CKEYVALUESLEXER_H
#define CKEYVALUESLEXER_H

#include "CMemory.h"
#include "CString.h"

struct CKeyvaluesLexerSettings
{
	CKeyvaluesLexerSettings(bool allowAnonymousBlocks = false,
		bool reportBlockErrors = true, bool reportUnclosedQuotes = true)
	{
		m_bAllowAnonymousBlocks = allowAnonymousBlocks;
		m_bReportBlockErrors = reportBlockErrors;
		m_bReportUnclosedQuotes = reportUnclosedQuotes;
	}

	bool	m_bAllowAnonymousBlocks;
	bool	m_bReportBlockErrors;
	bool	m_bReportUnclosedQuotes;
};

class CKeyvaluesLexer
{
public:
	enum TokenType
	{
		TOKEN_NONE = 0,
		TOKEN_BLOCK_OPEN,
		TOKEN_BLOCK_CLOSE,
		TOKEN_KEY,
		TOKEN_VALUE
	};

	enum ReadResult
	{
		READ_SUCCESS = 0,
		READ_END_OF_DATA,
		READ_FORMAT_ERROR
	};

	CKeyvaluesLexer(const CKeyvaluesLexerSettings &settings = CKeyvaluesLexerSettings());
	CKeyvaluesLexer(CMemory<unsigned int> &input,
		const CKeyvaluesLexerSettings &settings = CKeyvaluesLexerSettings());
	CKeyvaluesLexer(const char *fileName,
		const CKeyvaluesLexerSettings &settings = CKeyvaluesLexerSettings());
	CKeyvaluesLexer(const char *fileName, const char *pathID,
		const CKeyvaluesLexerSettings &settings = CKeyvaluesLexerSettings());
	~CKeyvaluesLexer();

	bool HasInputData(void) const;
	unsigned int GetReadOffset(void) const;
	void Reset(void);
	void Swap(CKeyvaluesLexer &lexer);
	bool IsValidReadPosition(void);
	void SkipWhitespace(void);
	bool SkipCommentLine(void);
	bool SkipComments(void);
	ReadResult ReadNext(const char *&start, const char *&end, bool &quoted);
	ReadResult ReadNextToken(void);
	ReadResult Read(void);

	TokenType GetTokenType(void) const { return m_TokenType; }
	const CString &GetToken(void) const { return m_Token; }

private:
	CMemory<unsigned int>		m_Input;
	const char					*m_pszReadPosition;
	TokenType					m_TokenType;
	CString						m_Token;
	CKeyvaluesLexerSettings		m_Settings;
};

#endif
