#ifndef CASBASESCRIPTGLOBAL_H
#define CASBASESCRIPTGLOBAL_H

#include "CASServerManager.h"
#include <stddef.h>

template<class Interface, class T>
class CASBaseScriptGlobal : public Interface
{
public:
	CASBaseScriptGlobal()
	{
		CASServerManager::GetInstance()->AddScriptGlobal(this);
	}

	virtual ~CASBaseScriptGlobal()
	{
		CASServerManager::GetInstance()->RemoveScriptGlobal(this);
	}

	static T *GetInstance(void)
	{
		if (!ms_pInstance)
			ms_pInstance = new T;

		return ms_pInstance;
	}

	virtual void Initialize(void) {}
	virtual void Shutdown(void) {}
	virtual void Think(void) {}

protected:
	static T	*ms_pInstance;
};

template<class Interface, class T>
T *CASBaseScriptGlobal<Interface, T>::ms_pInstance;

#endif
