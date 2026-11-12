#include "server/CMapPreprocessor.h"
#include "utils/CKeyvaluesParser.h"

CBaseEntityPreprocessor::CBaseEntityPreprocessor()
{
}

CBaseEntityPreprocessor::~CBaseEntityPreprocessor()
{
}

bool CBaseEntityPreprocessor::ShouldPreprocess(const char *className) const
{
	return true;
}

void CBaseEntityPreprocessor::PreprocessStateChanged(PreprocessState state)
{
}

CMapPreprocessor::CMapPreprocessor(CBaseEntityPreprocessor **preprocessors, unsigned int count)
	: m_pPreprocessors(preprocessors), m_uiPreprocessors(count)
{
}

CMapPreprocessor::~CMapPreprocessor()
{
}

void CMapPreprocessor::PreprocessEntity(const CKvBlockNode &entity)
{
	CString className;

	for (unsigned int i = 0; i < m_uiPreprocessors; i++)
	{
		className = entity.FindFirstKeyvalue("classname");

		if (m_pPreprocessors[i]->ShouldPreprocess(className.Data()))
			m_pPreprocessors[i]->PreprocessEntity(className, entity);
	}
}

void CMapPreprocessor::NotifyStateChanged(PreprocessState state)
{
	for (unsigned int i = 0; i < m_uiPreprocessors; i++)
		m_pPreprocessors[i]->PreprocessStateChanged(state);
}

void CMapPreprocessor::Preprocess(const char *data, unsigned int size)
{
	std::shared_ptr<CKvBlockNode> entity;
	CKeyvaluesParserSettings settings(true, true, true, false);
	CIterativeKeyvaluesParser parser(std::vector<char>(data, data + size), settings);

	NotifyStateChanged(PREPROCESS_BEGIN);

	while (parser.ParseBlock(entity) == CBaseKeyvaluesParser::PARSE_SUCCESS)
		PreprocessEntity(*entity);

	NotifyStateChanged(PREPROCESS_END);
}
