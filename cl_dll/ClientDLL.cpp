#include "hud.h"
#include "cl_util.h"
#include "ClientDLL.h"
#include "client_sound.h"
#include "client_support.h"
#include "portal_manager.h"

IEngineClient *g_pEngineClient;
IClientDLL *g_pClientDLL;
CClientDLL *CClientDLL::s_pClientDLL;

CClientDLL::CClientDLL()
{
	s_pClientDLL = this;
	g_pClientDLL = this;
	m_pEngineFactory = NULL;
}

CClientDLL::~CClientDLL()
{
}

void CClientDLL::Init(CreateInterfaceFn factory)
{
	m_pEngineFactory = factory;

	if (!m_pEngineFactory)
		UTIL_HostError("Could not get Svengine factory.\n", -1);

	g_pEngineClient = (IEngineClient *)m_pEngineFactory(ENGINECLIENT_INTERFACE_VERSION, NULL);

	if (!g_pEngineClient)
		UTIL_HostError("Unable to initialize Svengine client interface.\n", -1);
}

void CClientDLL::PlaySoundFX(float *origin, int channelIndex, const char *sample,
	float volume, float attenuation, int flags, int pitch)
{
	CClient_SoundEngine::GetSoundEngine()->PlayFMODSound(0, 0, origin, channelIndex, sample,
		volume, attenuation, flags, pitch, -1, 0);
}

void CClientDLL::RenderBegin(ref_params_s *params)
{
	P_PortalRender(params);
}

void CClientDLL::OnRenderViewDone(void)
{
}

void CClientDLL::EndFrame(void)
{
}

EXPOSE_SINGLE_INTERFACE(CClientDLL, IClientDLL, CLIENTDLL_INTERFACE_VERSION);
