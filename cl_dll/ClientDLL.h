#ifndef CLIENTDLL_H
#define CLIENTDLL_H

#include "IClient.h"

class CClientDLL : public IClientDLL
{
public:
	CClientDLL();
	virtual ~CClientDLL();
	virtual void Init(CreateInterfaceFn factory);
	virtual void PlaySoundFX(float *origin, int channelIndex, const char *sample,
		float volume, float attenuation, int flags, int pitch);
	virtual void RenderBegin(ref_params_s *params);
	virtual void OnRenderViewDone(void);
	virtual void EndFrame(void);

	static CClientDLL	*s_pClientDLL;

private:
	CreateInterfaceFn	m_pEngineFactory;
};

#endif
