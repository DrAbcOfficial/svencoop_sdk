#ifndef IASSCRIPTGLOBAL_H
#define IASSCRIPTGLOBAL_H

class IASScriptGlobal
{
public:
	virtual ~IASScriptGlobal();

	virtual void Initialize(void) = 0;
	virtual void Shutdown(void) = 0;
	virtual void Think(void) = 0;
};

#endif
