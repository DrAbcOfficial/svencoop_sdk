#include "extdll.h"
#include "server/server_filesystem.h"
#include "interface.h"

IFileSystem *g_pFileSystem;
CSysModule *g_pFileSystemModule;

bool InitializeFileSystem(void)
{
#ifdef _WIN32
	g_pFileSystemModule = Sys_LoadModule("filesystem_stdio.dll");
#else
	g_pFileSystemModule = Sys_LoadModule("filesystem_stdio.so");
#endif

	if (!g_pFileSystemModule)
		return false;

	CreateInterfaceFn factory = Sys_GetFactory(g_pFileSystemModule);

	if (!factory)
		return false;

	g_pFileSystem = (IFileSystem *)factory("SCFileSystem002", NULL);
	return g_pFileSystem != NULL;
}

void CloseFileSystem(void)
{
	if (g_pFileSystemModule)
		Sys_UnloadModule(g_pFileSystemModule);
}

FileHandle_t OpenGameFile(const char *fileName, const char *options)
{
	return g_pFileSystem->Open(fileName, options, "GAME|DEFAULTGAME");
}
