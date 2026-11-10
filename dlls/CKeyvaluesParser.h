#ifndef CKEYVALUESPARSER_H
#define CKEYVALUESPARSER_H

#include "CKeyvaluesLexer.h"
#include "CKeyvalues.h"

struct CKeyvaluesParserSettings : public CKeyvaluesLexerSettings
{
	CKeyvaluesParserSettings(bool allowAnonymousBlocks = false,
		bool reportBlockErrors = true, bool reportUnclosedQuotes = true,
		bool allowSubblocks = true)
		: CKeyvaluesLexerSettings(allowAnonymousBlocks, reportBlockErrors, reportUnclosedQuotes),
		m_bAllowSubblocks(allowSubblocks)
	{
	}

	bool	m_bAllowSubblocks;
};

class CBaseKeyvaluesParser
{
public:
	enum ParseResult
	{
		PARSE_SUCCESS = 0,
		PARSE_END_OF_DATA,
		PARSE_UNEXPECTED_EOB,
		PARSE_FORMAT_ERROR,
		PARSE_UNKNOWN_ERROR,
		PARSE_WRONG_NODE_TYPE
	};

	CBaseKeyvaluesParser(const CKeyvaluesParserSettings &settings, bool iterative);
	CBaseKeyvaluesParser(CMemory<unsigned int> &input,
		const CKeyvaluesParserSettings &settings, bool iterative);
	CBaseKeyvaluesParser(const char *fileName,
		const CKeyvaluesParserSettings &settings, bool iterative);
	CBaseKeyvaluesParser(const char *fileName, const char *pathID,
		const CKeyvaluesParserSettings &settings, bool iterative);

	void Construct(void);
	unsigned int GetReadOffset(void) const;
	void Initialize(CMemory<unsigned int> &input);
	ParseResult GetResultFor(CKeyvaluesLexer::ReadResult result, bool unexpectedEnd) const;
	ParseResult ParseBlock(std::shared_ptr<CKvBlockNode> &block, bool root);
	ParseResult ParseNext(std::shared_ptr<CKeyvalueNode> &node, bool read);
	static const char *ParseResultToString(ParseResult result);

private:
	friend class CASPluginManager;

	CKeyvaluesLexer			m_Lexer;
	int						m_iDepth;
	CKeyvaluesParserSettings	m_Settings;
	bool					m_bIterative;
};

class CKeyvaluesParser : public CBaseKeyvaluesParser
{
public:
	CKeyvaluesParser(const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());
	CKeyvaluesParser(CMemory<unsigned int> &input,
		const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());
	CKeyvaluesParser(const char *fileName,
		const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());
	CKeyvaluesParser(const char *fileName, const char *pathID,
		const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());

	void Initialize(CMemory<unsigned int> &input);
	ParseResult Parse(void);
	const std::shared_ptr<CKeyvalues> &GetKeyvalues(void) const { return m_pKeyvalues; }

private:
	std::shared_ptr<CKeyvalues>	m_pKeyvalues;
};

class CIterativeKeyvaluesParser : public CBaseKeyvaluesParser
{
public:
	CIterativeKeyvaluesParser(const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());
	CIterativeKeyvaluesParser(CMemory<unsigned int> &input,
		const CKeyvaluesParserSettings &settings = CKeyvaluesParserSettings());

	ParseResult ParseBlock(std::shared_ptr<CKvBlockNode> &block);
};

#endif
