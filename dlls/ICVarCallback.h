#ifndef ICVARCALLBACK_H
#define ICVARCALLBACK_H

class CCVar;
class CString;

class ICVarCallback
{
public:
	virtual ~ICVarCallback() {}
	virtual void CVarCallback(CCVar &variable, const CString &oldValue, float oldFloat) = 0;
};

typedef void (*CVarCallback_t)(CCVar &variable, const CString &oldValue, float oldFloat);

#endif
