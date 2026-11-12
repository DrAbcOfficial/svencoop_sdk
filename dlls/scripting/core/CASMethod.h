#ifndef CASMETHOD_H
#define CASMETHOD_H

#include "scripting/core/CASBaseCallable.h"

class CASMethod : public CASBaseCallable
{
public:
	CASMethod(asIScriptFunction *function, CASModule *module);
	virtual ~CASMethod();

	void *GetThisPointer(void) const;
	void SetThisPointer(void *object);

protected:
	virtual bool PreExecute(void);
	virtual void PostExecute(void);

private:
	void	*m_pThisPointer;
};

#endif
