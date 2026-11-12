#ifndef ICUSTOMENTITY_H
#define ICUSTOMENTITY_H

class CASObjectInstance;
class CCustomEntityHandler;

class ICustomEntity
{
public:
	virtual CCustomEntityHandler *GetHandler() const = 0;
	virtual CASObjectInstance *GetScriptInstance() const = 0;
	virtual void SetScriptInstance( CASObjectInstance *instance ) = 0;
	virtual bool OnScriptClassReady()
	{
		return true;
	}
};

#endif
