#include "extdll.h"
#include "server/server_filesystem.h"
#include "scripting/core/CASFileSystemUtils.h"
#include "CString.h"

OpenFileFlags_t CASFileSystemUtils::FilterFlags(OpenFileFlags_t flags)
{
	if ((flags & (OpenFileFlagBit::WRITE | OpenFileFlagBit::APPEND)) == (OpenFileFlagBit::WRITE | OpenFileFlagBit::APPEND))
		flags &= ~OpenFileFlagBit::WRITE;

	return flags;
}

bool CASFileSystemUtils::FlagsValid(OpenFileFlags_t flags)
{
	if (!flags || !(flags & (OpenFileFlagBit::READ | OpenFileFlagBit::WRITE | OpenFileFlagBit::APPEND)))
		return false;

	if (flags & OpenFileFlagBit::READ)
		return !(flags & (OpenFileFlagBit::WRITE | OpenFileFlagBit::APPEND));

	return true;
}

bool CASFileSystemUtils::FormatOpenFlags(OpenFileFlags_t flags, CString &mode)
{
	mode = "";

	if (!FlagsValid(flags))
		return false;

	flags = FilterFlags(flags);

	char text[sizeof("rb")] = { 0 };

	if (flags & OpenFileFlagBit::READ)
		text[0] = 'r';
	else if (flags & OpenFileFlagBit::WRITE)
		text[0] = 'w';
	else if (flags & OpenFileFlagBit::APPEND)
		text[0] = 'a';

	if (flags & OpenFileFlagBit::BINARY)
		text[1] = 'b';

	mode = text;
	return true;
}

void CASFileSystemUtils::CreateDirectory(const char *directory)
{
	if (directory && *directory)
		g_pFileSystem->CreateDirHierarchy(directory, "GAMECONFIG");
}

void CASFileSystemUtils::RemoveFile(const char *fileName)
{
	if (fileName && *fileName)
		g_pFileSystem->RemoveFile(fileName, "GAMECONFIG");
}
