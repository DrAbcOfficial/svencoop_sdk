#ifndef PORTAL_GL_H
#define PORTAL_GL_H

#include <SDL2/SDL_opengl.h>

extern PFNGLACTIVETEXTUREPROC portalActiveTexture;
extern PFNGLGENBUFFERSPROC portalGenBuffers;
extern PFNGLDELETEBUFFERSPROC portalDeleteBuffers;
extern PFNGLBINDBUFFERPROC portalBindBuffer;
extern PFNGLBUFFERDATAPROC portalBufferData;
extern PFNGLCREATESHADERPROC portalCreateShader;
extern PFNGLSHADERSOURCEPROC portalShaderSource;
extern PFNGLCOMPILESHADERPROC portalCompileShader;
extern PFNGLCREATEPROGRAMPROC portalCreateProgram;
extern PFNGLATTACHSHADERPROC portalAttachShader;
extern PFNGLLINKPROGRAMPROC portalLinkProgram;
extern PFNGLUSEPROGRAMPROC portalUseProgram;
extern PFNGLDELETEPROGRAMPROC portalDeleteProgram;
extern PFNGLDELETESHADERPROC portalDeleteShader;

void InitializePortalGL(void);
bool SetActiveTexture(unsigned int texture);
bool CancelPortalsDueToEngineFBOBeingSet(void);

#endif
