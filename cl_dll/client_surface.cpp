#include "hud.h"
#include "cl_util.h"
#include "com_model.h"
#include "r_studioint.h"
#include "triangleapi.h"
#include "IClient.h"
#include "portal_surface.h"
#include "client_surface.h"

#include <SDL2/SDL_opengl.h>
#include <string.h>

extern engine_studio_api_t IEngineStudio;

CNovodexAPI *CNovodexAPI::m_pAPI;
CSurfaceHandler *CSurfaceHandler::m_pAPI;
bool m_fFinishedNovodex;

CNovodexAPI *CNovodexAPI::GetNovodexAPI(void)
{
	if (!m_pAPI)
		m_pAPI = new CNovodexAPI;

	return m_pAPI;
}

CSurfaceHandler::CSurfaceHandler()
{
	m_pSurfaces = NULL;
}

CSurfaceHandler *CSurfaceHandler::GetSurfaceHandler(void)
{
	if (!m_pAPI)
		m_pAPI = new CSurfaceHandler;

	return m_pAPI;
}

CSurfaceHandler *gSurfaceHandler(void)
{
	return CSurfaceHandler::GetSurfaceHandler();
}

void CSurfaceHandler::DrawKnownSurfaces(void)
{
	if (!m_fFinishedNovodex)
		return;

	for (surfacelist_t *node = m_pSurfaces; node; node = node->next)
	{
		msurface_s *surface = node->surface;
		glpoly_s *poly = surface->polys;
		model_s *sprite = (model_s *)g_pEngineClient->LoadClientModel("sprites/redflare1.spr");

		if (!sprite || !gEngfuncs.pTriAPI->SpriteTexture(sprite, 0))
		{
			gEngfuncs.Con_Printf("Couldn't get sprite pointer!\n");
			return;
		}

		gEngfuncs.pTriAPI->CullFace(TRI_FRONT);
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 200);
		gEngfuncs.pTriAPI->Begin(TRI_POLYGON);

		for (int i = 0; i < poly->numverts; i++)
		{
			float *vertex = poly->verts[i];

			glNormal3f(surface->plane->normal[0], surface->plane->normal[1], surface->plane->normal[2]);
			gEngfuncs.pTriAPI->TexCoord2f(vertex[3], -vertex[4]);
			gEngfuncs.pTriAPI->Vertex3f(vertex[0], vertex[1], vertex[2]);
		}

		gEngfuncs.pTriAPI->End();
	}
}

bool CSurfaceHandler::HaveSeenSurface(msurface_s *surface)
{
	for (surfacelist_t *node = m_pSurfaces; node; node = node->next)
	{
		if (node->surface == surface)
			return true;
	}

	return false;
}

void CSurfaceHandler::RememberSurface(msurface_s *surface)
{
	surfacelist_t *added = new surfacelist_t;

	added->surface = surface;
	added->next = NULL;

	if (!m_pSurfaces)
	{
		m_pSurfaces = added;
		return;
	}

	surfacelist_t *last = m_pSurfaces;

	while (last->next)
		last = last->next;

	last->next = added;
}

void CSurfaceHandler::ParseLeafForSurfaces(mleaf_s *leaf, const char *texture)
{
	if (!IEngineStudio.IsHardware())
		return;

	for (int i = 0; i < leaf->nummarksurfaces; i++)
	{
		msurface_s *surface = leaf->firstmarksurface[i];

		if (HaveSeenSurface(surface))
			continue;

		if (texture && strcmp(surface->texinfo->texture->name, texture))
			continue;

		RememberSurface(surface);
	}
}

void CSurfaceHandler::ForgetAllSurfaces(void)
{
	m_pSurfaces = NULL;
}

void CSurfaceHandler::Novodex_InitializeScene(void)
{
	if (m_fFinishedNovodex)
		return;

	gSurfaceHandler()->ForgetAllSurfaces();

	model_s *world = IEngineStudio.GetModelByIndex(1);

	if (!world)
		return;

	for (int i = 0; i < world->numleafs; i++)
		ParseLeafForSurfaces(&world->leafs[i], NULL);

	m_fFinishedNovodex = true;
}

void CSurfaceHandler::FMOD_InitializeScene(void)
{
	if (m_fFinishedNovodex)
		return;

	gSurfaceHandler()->ForgetAllSurfaces();

	model_s *world = IEngineStudio.GetModelByIndex(1);

	if (!world)
		return;

	for (int i = 0; i < world->numleafs; i++)
		ParseLeafForSurfaces(&world->leafs[i], NULL);

	for (surfacelist_t *node = m_pSurfaces; node; node = node->next)
	{
		msurface_s *surface = node->surface;
		glpoly_s *poly = surface->polys;

		for (int i = 0; i < poly->numverts; i++)
		{
			float *vertex = poly->verts[i];

			glNormal3f(surface->plane->normal[0], surface->plane->normal[1], surface->plane->normal[2]);
			gEngfuncs.pTriAPI->Vertex3f(vertex[0], vertex[1], vertex[2]);
		}
	}

	m_fFinishedNovodex = true;
}
