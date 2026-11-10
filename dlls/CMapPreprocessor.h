#ifndef CMAPPREPROCESSOR_H
#define CMAPPREPROCESSOR_H

class CString;
class CKvBlockNode;

enum PreprocessState
{
	PREPROCESS_BEGIN = 0,
	PREPROCESS_END
};

class CBaseEntityPreprocessor
{
public:
	CBaseEntityPreprocessor();
	virtual ~CBaseEntityPreprocessor();
	virtual bool ShouldPreprocess(const char *className) const;
	virtual void PreprocessEntity(const CString &className, const CKvBlockNode &entity) = 0;
	virtual void PreprocessStateChanged(PreprocessState state);
};

class CMapPreprocessor
{
public:
	CMapPreprocessor(CBaseEntityPreprocessor **preprocessors, unsigned int count);
	virtual ~CMapPreprocessor();

	void PreprocessEntity(const CKvBlockNode &entity);
	void NotifyStateChanged(PreprocessState state);
	void Preprocess(const char *data, unsigned int size);

private:
	CBaseEntityPreprocessor	**m_pPreprocessors;
	unsigned int			m_uiPreprocessors;
};

#endif
