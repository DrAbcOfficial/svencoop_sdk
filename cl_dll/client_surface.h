#ifndef CLIENT_SURFACE_H
#define CLIENT_SURFACE_H

struct msurface_s;
struct mleaf_s;

struct surfacelist_t
{
	msurface_s		*surface;
	surfacelist_t	*next;
};

class CNovodexAPI
{
public:
	static CNovodexAPI *GetNovodexAPI(void);

	static CNovodexAPI	*m_pAPI;
};

class CSurfaceHandler
{
public:
	CSurfaceHandler();

	static CSurfaceHandler *GetSurfaceHandler(void);
	void DrawKnownSurfaces(void);
	bool HaveSeenSurface(msurface_s *surface);
	void RememberSurface(msurface_s *surface);
	void ParseLeafForSurfaces(mleaf_s *leaf, const char *texture);
	void ForgetAllSurfaces(void);
	void Novodex_InitializeScene(void);
	void FMOD_InitializeScene(void);

	static CSurfaceHandler	*m_pAPI;

	surfacelist_t	*m_pSurfaces;
};

CSurfaceHandler *gSurfaceHandler(void);

extern bool m_fFinishedNovodex;

#endif
