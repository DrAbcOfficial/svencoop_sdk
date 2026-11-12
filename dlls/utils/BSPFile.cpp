#include "extdll.h"
#include "server/server_filesystem.h"
#include "utils/BSPFile.h"

dheader_s *LoadBSPFile(const char *fileName, std::vector<char> &memory)
{
	FileHandle_t file = OpenGameFile(fileName, "rb");

	if (!file)
		return NULL;

	unsigned int position = g_pFileSystem->Tell(file);

	g_pFileSystem->Seek(file, 0, FILESYSTEM_SEEK_TAIL);

	unsigned int size = g_pFileSystem->Tell(file);

	g_pFileSystem->Seek(file, position, FILESYSTEM_SEEK_HEAD);

	memory.resize(size);
	int bytesRead = g_pFileSystem->Read(memory.data(), size, file);
	dheader_s *header = NULL;

	if (bytesRead == size)
	{
		header = reinterpret_cast<dheader_s *>(memory.data());
	}
	else
	{
		memory.clear();
	}

	g_pFileSystem->Close(file);
	return header;
}
