#include "extdll.h"
#include "CKeyvaluesParser.h"

CBaseKeyvaluesParser::CBaseKeyvaluesParser(const CKeyvaluesParserSettings &settings, bool iterative)
	: m_Lexer(settings), m_iDepth(0), m_Settings(settings), m_bIterative(iterative)
{
	Construct();
}

CBaseKeyvaluesParser::CBaseKeyvaluesParser(CMemory<unsigned int> &input,
	const CKeyvaluesParserSettings &settings, bool iterative)
	: m_Lexer(input, settings), m_iDepth(0), m_Settings(settings), m_bIterative(iterative)
{
	Construct();
}

CBaseKeyvaluesParser::CBaseKeyvaluesParser(const char *fileName,
	const CKeyvaluesParserSettings &settings, bool iterative)
	: m_Lexer(fileName, settings), m_iDepth(0), m_Settings(settings), m_bIterative(iterative)
{
	Construct();
}

CBaseKeyvaluesParser::CBaseKeyvaluesParser(const char *fileName, const char *pathID,
	const CKeyvaluesParserSettings &settings, bool iterative)
	: m_Lexer(fileName, pathID, settings), m_iDepth(0), m_Settings(settings), m_bIterative(iterative)
{
	Construct();
}

void CBaseKeyvaluesParser::Construct(void)
{
	if (m_bIterative)
		m_iDepth++;
}

unsigned int CBaseKeyvaluesParser::GetReadOffset(void) const
{
	return m_Lexer.GetReadOffset();
}

void CBaseKeyvaluesParser::Initialize(CMemory<unsigned int> &input)
{
	CKeyvaluesLexer lexer(input, m_Settings);

	m_Lexer.Swap(lexer);
	m_iDepth = m_bIterative ? 1 : 0;
}

CBaseKeyvaluesParser::ParseResult CBaseKeyvaluesParser::GetResultFor(
	CKeyvaluesLexer::ReadResult result, bool unexpectedEnd) const
{
	if (result == CKeyvaluesLexer::READ_END_OF_DATA)
		return m_iDepth > 1 || unexpectedEnd ? PARSE_UNEXPECTED_EOB : PARSE_END_OF_DATA;

	if (result != CKeyvaluesLexer::READ_SUCCESS)
		return result == CKeyvaluesLexer::READ_FORMAT_ERROR ? PARSE_FORMAT_ERROR : PARSE_UNKNOWN_ERROR;

	return PARSE_SUCCESS;
}

CBaseKeyvaluesParser::ParseResult CBaseKeyvaluesParser::ParseBlock(
	std::shared_ptr<CKvBlockNode> &block, bool root)
{
	m_iDepth++;

	CKvBlockNode::Children children;
	std::shared_ptr<CKeyvalueNode> node;

	while (true)
	{
		CKeyvaluesLexer::ReadResult readResult = m_Lexer.Read();

		if (readResult == CKeyvaluesLexer::READ_END_OF_DATA && root)
			m_iDepth--;

		ParseResult result = GetResultFor(readResult, false);

		switch (m_Lexer.GetTokenType())
		{
		case CKeyvaluesLexer::TOKEN_BLOCK_CLOSE:
			if (root)
				return PARSE_FORMAT_ERROR;

			m_iDepth--;
			block->SetChildren(children);
			return result;

		case CKeyvaluesLexer::TOKEN_NONE:
			if (!root)
				return PARSE_FORMAT_ERROR;

			block->SetChildren(children);
			return result;

		default:
			ParseResult nodeResult = ParseNext(node, false);

			if (nodeResult != PARSE_SUCCESS)
				return nodeResult;

			children.push_back(node);

			if (result != PARSE_SUCCESS)
				return PARSE_SUCCESS;
		}
	}
}

CBaseKeyvaluesParser::ParseResult CBaseKeyvaluesParser::ParseNext(
	std::shared_ptr<CKeyvalueNode> &node, bool read)
{
	if (read)
	{
		ParseResult result = GetResultFor(m_Lexer.Read(), false);

		if (result != PARSE_SUCCESS)
			return result;
	}

	bool unnamed = m_Lexer.GetTokenType() != CKeyvaluesLexer::TOKEN_KEY;

	if (unnamed && !m_Settings.m_bAllowAnonymousBlocks)
		return PARSE_FORMAT_ERROR;

	CString key(unnamed ? CString("UNNAMED") : m_Lexer.GetToken());

	if (!unnamed)
	{
		CKeyvaluesLexer::ReadResult readResult = m_Lexer.Read();
		ParseResult result = GetResultFor(readResult, readResult == CKeyvaluesLexer::READ_SUCCESS);

		if (result != PARSE_SUCCESS)
			return result;
	}

	switch (m_Lexer.GetTokenType())
	{
	case CKeyvaluesLexer::TOKEN_BLOCK_OPEN:
	{
		if (m_iDepth != 1 && !m_Settings.m_bAllowSubblocks)
			return PARSE_FORMAT_ERROR;

		node = std::make_shared<CKvBlockNode>(key.Data());

		std::shared_ptr<CKvBlockNode> block = std::static_pointer_cast<CKvBlockNode>(node);

		return ParseBlock(block, false);
	}

	case CKeyvaluesLexer::TOKEN_VALUE:
		node = std::make_shared<CKeyvalue>(key.Data(), m_Lexer.GetToken().Data());
		return PARSE_SUCCESS;

	default:
		return PARSE_FORMAT_ERROR;
	}
}

const char *CBaseKeyvaluesParser::ParseResultToString(ParseResult result)
{
	switch (result)
	{
	case PARSE_SUCCESS:
		return "Success";
	case PARSE_END_OF_DATA:
		return "EndOfData";
	case PARSE_UNEXPECTED_EOB:
		return "UnexpectedEOB";
	case PARSE_FORMAT_ERROR:
		return "FormatError";
	case PARSE_WRONG_NODE_TYPE:
		return "WrongNodeType";
	default:
		return "UnknownError";
	}
}

CKeyvaluesParser::CKeyvaluesParser(const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(settings, false)
{
}

CKeyvaluesParser::CKeyvaluesParser(CMemory<unsigned int> &input,
	const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(input, settings, false)
{
}

CKeyvaluesParser::CKeyvaluesParser(const char *fileName,
	const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(fileName, settings, false)
{
}

CKeyvaluesParser::CKeyvaluesParser(const char *fileName, const char *pathID,
	const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(fileName, pathID, settings, false)
{
}

void CKeyvaluesParser::Initialize(CMemory<unsigned int> &input)
{
	CBaseKeyvaluesParser::Initialize(input);
	m_pKeyvalues.reset();
}

CBaseKeyvaluesParser::ParseResult CKeyvaluesParser::Parse(void)
{
	std::shared_ptr<CKvBlockNode> root = std::make_shared<CKvBlockNode>("ROOT_NODE");
	ParseResult result = ParseBlock(root, true);

	if (result != PARSE_UNEXPECTED_EOB)
		m_pKeyvalues = std::make_shared<CKeyvalues>(root);

	return result == PARSE_END_OF_DATA ? PARSE_SUCCESS : result;
}

CIterativeKeyvaluesParser::CIterativeKeyvaluesParser(const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(settings, true)
{
}

CIterativeKeyvaluesParser::CIterativeKeyvaluesParser(CMemory<unsigned int> &input,
	const CKeyvaluesParserSettings &settings)
	: CBaseKeyvaluesParser(input, settings, true)
{
}

CBaseKeyvaluesParser::ParseResult CIterativeKeyvaluesParser::ParseBlock(std::shared_ptr<CKvBlockNode> &block)
{
	block.reset();

	std::shared_ptr<CKeyvalueNode> node;
	ParseResult result = ParseNext(node, true);

	if (result != PARSE_SUCCESS)
		return result;

	if (node->GetType() != KEYVALUE_NODE_BLOCK)
		return PARSE_WRONG_NODE_TYPE;

	block = std::static_pointer_cast<CKvBlockNode>(node);
	return PARSE_SUCCESS;
}
