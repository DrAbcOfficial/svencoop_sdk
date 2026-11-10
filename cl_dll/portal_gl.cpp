#include "portal_gl.h"
#include <SDL2/SDL.h>

PFNGLACTIVETEXTUREPROC portalActiveTexture;
PFNGLGENBUFFERSPROC portalGenBuffers;
PFNGLDELETEBUFFERSPROC portalDeleteBuffers;
PFNGLBINDBUFFERPROC portalBindBuffer;
PFNGLBUFFERDATAPROC portalBufferData;
PFNGLCREATESHADERPROC portalCreateShader;
PFNGLSHADERSOURCEPROC portalShaderSource;
PFNGLCOMPILESHADERPROC portalCompileShader;
PFNGLCREATEPROGRAMPROC portalCreateProgram;
PFNGLATTACHSHADERPROC portalAttachShader;
PFNGLLINKPROGRAMPROC portalLinkProgram;
PFNGLUSEPROGRAMPROC portalUseProgram;
PFNGLDELETEPROGRAMPROC portalDeleteProgram;
PFNGLDELETESHADERPROC portalDeleteShader;

void InitializePortalGL(void)
{
	portalActiveTexture = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");
	portalGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
	portalDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
	portalBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
	portalBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
	portalCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
	portalShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
	portalCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
	portalCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
	portalAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
	portalLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
	portalUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
	portalDeleteProgram = (PFNGLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");
	portalDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
}

bool SetActiveTexture(unsigned int texture)
{
	GLint activeTexture = 0;

	if (portalActiveTexture)
		portalActiveTexture(texture);

	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
	return activeTexture == texture;
}
