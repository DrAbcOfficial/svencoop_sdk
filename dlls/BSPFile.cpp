#include "extdll.h"
#include "server_filesystem.h"
#include "BSPFile.h"

dheader_s *LoadBSPFile(const char *fileName, CMemory<unsigned int> &memory)
{
	FileHandle_t file = OpenGameFile(fileName, "rb");

	if (!file)
		return NULL;

	unsigned int position = g_pFileSystem->Tell(file);

	g_pFileSystem->Seek(file, 0, FILESYSTEM_SEEK_TAIL);

	unsigned int size = g_pFileSystem->Tell(file);

	g_pFileSystem->Seek(file, position, FILESYSTEM_SEEK_HEAD);

	char *data = new char[size + 1];
	int bytesRead = g_pFileSystem->Read(data, size, file);
	dheader_s *header = NULL;

	if (bytesRead == size)
	{
		CMemory<unsigned int> input(data, size, true);

		memory.Swap(input);
		header = (dheader_s *)memory.Data();
	}
	else
	{
		delete[] data;
	}

	g_pFileSystem->Close(file);
	return header;
}
