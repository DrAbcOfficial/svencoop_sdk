#include "extdll.h"
#include "enginecallback.h"
#include "server/server_filesystem.h"
#include "utils/CEscapeSequences.h"
#include "utils/CKeyvalues.h"
#include "utils/CKeyvaluesWriter.h"

CKeyvaluesWriter::CKeyvaluesWriter(const char *fileName, const CKeyvaluesLexerSettings &settings)
	: m_Settings(settings), m_pConversions(&getNoEscapeSeqConversion()), m_hFile(NULL),
	m_uiDepth(0), m_bError(false)
{
	m_szFileName[0] = 0;
	Open(fileName);
}

CKeyvaluesWriter::CKeyvaluesWriter(const char *fileName, CEscapeSequences &conversions,
	const CKeyvaluesLexerSettings &settings)
	: m_Settings(settings), m_pConversions(&conversions), m_hFile(NULL),
	m_uiDepth(0), m_bError(false)
{
	m_szFileName[0] = 0;
	Open(fileName);
}

CKeyvaluesWriter::~CKeyvaluesWriter()
{
	Close();
}

bool CKeyvaluesWriter::Open(const char *fileName)
{
	Close();

	if (!fileName)
		return false;

	m_hFile = g_pFileSystem->Open(fileName, "w", "GAMECONFIG");
	g_pFileSystem->GetLocalPath(fileName, m_szFileName, sizeof(m_szFileName));
	return m_hFile != NULL;
}

void CKeyvaluesWriter::Close(void)
{
	if (m_hFile)
	{
		g_pFileSystem->Close(m_hFile);
		m_hFile = NULL;
		m_szFileName[0] = 0;
	}

	m_bError = false;
}

void CKeyvaluesWriter::Error(const char *message)
{
	if (m_Settings.m_bReportBlockErrors)
		ALERT(at_error, "%s", message);

	m_bError = true;
}

bool CKeyvaluesWriter::WriteTabs(void)
{
	if (m_uiDepth >= WRITE_BUFFER_SIZE)
	{
		Error("CKeyvaluesWriter::PrintTabs: Tabs too large to output!\n");
		return false;
	}

	char buffer[WRITE_BUFFER_SIZE];

	memset(buffer, '\t', m_uiDepth);
	buffer[m_uiDepth] = 0;
	g_pFileSystem->Write(buffer, m_uiDepth, m_hFile);
	return true;
}

bool CKeyvaluesWriter::BeginBlock(const char *key)
{
	if (m_bError)
		return false;

	if (!m_hFile)
	{
		Error("CKeyvaluesWriter::BeginBlock: No file open!\n");
		return false;
	}

	if (!key)
		key = "";

	if (!*key && !m_Settings.m_bAllowAnonymousBlocks)
	{
		Error("CKeyvaluesWriter::BeginBlock: No unnamed blocks allowed!\n");
		return false;
	}

	if (!WriteTabs() || !WriteToken(key))
		return false;

	g_pFileSystem->Write("\n", 1, m_hFile);

	if (!WriteTabs())
		return false;

	g_pFileSystem->Write("{\n", 2, m_hFile);
	m_uiDepth++;
	return true;
}

bool CKeyvaluesWriter::EndBlock(void)
{
	if (m_bError)
		return false;

	if (!m_hFile)
	{
		Error("CKeyvaluesWriter::EndBlock: No file open!\n");
		return false;
	}

	if (!m_uiDepth)
	{
		Error("CKeyvaluesWriter::EndBlock: No block open!\n");
		return false;
	}

	m_uiDepth--;

	if (!WriteTabs())
		return false;

	g_pFileSystem->Write("}\n", 2, m_hFile);
	return true;
}

bool CKeyvaluesWriter::WriteToken(const char *text)
{
	char buffer[WRITE_BUFFER_SIZE];
	bool quoted = !*text || strchr(text, ' ') != NULL;
	unsigned int size = quoted ? 1 : 0;
	unsigned int length = strlen(text);

	if (quoted)
		buffer[0] = '"';

	for (unsigned int i = 0; i < length; i++)
	{
		const char *conversion = m_pConversions->GetString(text[i]);

		if (conversion)
		{
			unsigned int conversionLength = m_pConversions->GetStringLength(text[i]);

			if (size + conversionLength >= WRITE_BUFFER_SIZE)
			{
				Error("CKeyvaluesWriter::WriteToken: Token too large!\n");
				return false;
			}

			strncpy(buffer + size, conversion, conversionLength);
			size += conversionLength;
		}
		else
		{
			buffer[size++] = text[i];

			if (size >= WRITE_BUFFER_SIZE)
			{
				Error("CKeyvaluesWriter::WriteToken: Token too large!\n");
				return false;
			}
		}
	}

	if (quoted)
		buffer[size++] = '"';

	if (size >= WRITE_BUFFER_SIZE)
	{
		Error("CKeyvaluesWriter::WriteToken: Token too large!\n");
		return false;
	}

	buffer[size] = 0;
	g_pFileSystem->Write(buffer, size, m_hFile);
	return true;
}

bool CKeyvaluesWriter::WriteKeyvalue(const char *key, const char *value)
{
	if (m_bError)
		return false;

	if (!m_hFile)
	{
		Error("CKeyvaluesWriter::WriteKeyValue: No file open!\n");
		return false;
	}

	if (!key || !*key)
	{
		Error("CKeyvaluesWriter::WriteKeyValue: Null or empty key!\n");
		return false;
	}

	if (!WriteTabs() || !WriteToken(key))
		return false;

	g_pFileSystem->Write(" ", 1, m_hFile);

	if (!WriteToken(value ? value : ""))
		return false;

	g_pFileSystem->Write("\n", 1, m_hFile);
	return true;
}

bool CKeyvaluesWriter::WriteKeyvalue(const CKeyvalue &keyvalue)
{
	return WriteKeyvalue(keyvalue.GetKey().Data(), keyvalue.GetValue().Data());
}

bool CKeyvaluesWriter::Write(const CKeyvalueNode &node)
{
	switch (node.GetType())
	{
	case KEYVALUE_NODE_VALUE:
		return WriteKeyvalue(static_cast<const CKeyvalue &>(node));
	case KEYVALUE_NODE_BLOCK:
		return WriteBlock(static_cast<const CKvBlockNode &>(node));
	default:
		Error("CKeyvaluesWriter::Write: Unknown node type!\n");
		return false;
	}
}

bool CKeyvaluesWriter::WriteBlock(const CKvBlockNode &block)
{
	if (!BeginBlock(block.GetKey().Data()))
		return false;

	const CKvBlockNode::Children &children = block.GetChildren();

	for (CKvBlockNode::Children::const_iterator it = children.begin(); it != children.end(); it++)
		Write(**it);

	return EndBlock();
}
