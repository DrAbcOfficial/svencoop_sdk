#ifndef CASSCRIPTENTITYPREPROCESSOR_H
#define CASSCRIPTENTITYPREPROCESSOR_H

#include "CMapPreprocessor.h"

class CASScriptEntityPreprocessor : public CBaseEntityPreprocessor
{
public:
	virtual ~CASScriptEntityPreprocessor();
	virtual bool ShouldPreprocess( const char *className ) const;
	virtual void PreprocessEntity( const CString &className, const CKvBlockNode &entity );
	virtual void PreprocessStateChanged( PreprocessState state );
};

#endif
