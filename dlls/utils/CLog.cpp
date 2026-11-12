#include "extdll.h"
#include "util.h"
#include "server/server_filesystem.h"
#include "server/server_support.h"
#include "utils/CLog.h"
#include <algorithm>
#include <string>
#include <vector>

bool LogFilenameLess(const std::string &first, const std::string &second)
{
	return stricmp(first.c_str(), second.c_str()) < 0;
}

CLog::CLog(const char *directory, const char *fileName)
{
	m_pFile = NULL;

	if (!directory || !fileName)
		ALERT(at_error, "CLog::CLog: must pass a valid directory and file name!\n");

	UTIL_CopyString(m_szDirectory, directory, sizeof(m_szDirectory));
	UTIL_CopyString(m_szFileName, fileName, sizeof(m_szFileName));
}

CLog::~CLog()
{
	Close();
}

bool CLog::Open(bool limitFiles, unsigned int count)
{
	Close();

	if (limitFiles)
		LimitToAmount(count);

	char fileName[MAX_DIRECTORY_LENGTH];

	if (!CreateFilename(fileName, sizeof(fileName)))
		return false;

	g_pFileSystem->CreateDirHierarchy(m_szDirectory, "GAMECONFIG");
	m_pFile = g_pFileSystem->Open(fileName, "a", "GAMECONFIG");
	return m_pFile != NULL;
}

void CLog::Close(void)
{
	if (m_pFile)
	{
		g_pFileSystem->Close(m_pFile);
		m_pFile = NULL;
	}
}

void CLog::Printf(const char *format, ...)
{
	va_list args;

	va_start(args, format);
	VPrintf(format, args);
	va_end(args);
}

void CLog::VPrintf(const char *format, va_list args)
{
	if (!format || !m_pFile)
		return;

	char buffer[MAX_MESSAGE_LENGTH];
	int length = vsnprintf(buffer, sizeof(buffer), format, args);

	if (length > 0 && (unsigned int)length < sizeof(buffer))
	{
		g_pFileSystem->FPrintf(m_pFile, "%s", buffer);
		g_pFileSystem->Flush(m_pFile);
	}
}

bool CLog::CreateFilename(char *buffer, unsigned int size, tm *time)
{
	if (!buffer || !size || !time)
		return false;

	char date[MAX_TIME_LENGTH];

	if (!strftime(date, sizeof(date), "%Y-%m-%d", time))
		return false;

	int length = snprintf(buffer, size, "%s/%s_%s.%s", m_szDirectory, m_szFileName, date, "log");

	return length >= 0 && (unsigned int)length < size;
}

bool CLog::CreateFilename(char *buffer, unsigned int size)
{
	if (!buffer || !size)
		return false;

	time_t now;

	time(&now);
	return CreateFilename(buffer, size, localtime(&now));
}

unsigned int CLog::LimitToAmount(unsigned int count)
{
	char pattern[MAX_DIRECTORY_LENGTH];
	std::vector<std::string> files;
	FileFindHandle_t handle = FILESYSTEM_INVALID_FIND_HANDLE;

	snprintf(pattern, sizeof(pattern), "%s/%s*.log", m_szDirectory, m_szFileName);

	const char *fileName = g_pFileSystem->FindFirst(pattern, &handle, "GAMECONFIG");

	if (fileName)
	{
		do
		{
			if (!g_pFileSystem->FindIsDirectory(handle))
				files.push_back(fileName);

			fileName = g_pFileSystem->FindNext(handle);
		}
		while (fileName);

		g_pFileSystem->FindClose(handle);
	}

	if (files.size() > count)
	{
		std::sort(files.begin(), files.end(), LogFilenameLess);

		unsigned int removeCount = files.size() - count;

		while (removeCount)
		{
			removeCount--;

			unsigned int index = removeCount;
			char path[MAX_DIRECTORY_LENGTH];
			int length = snprintf(path, sizeof(path), "%s/%s", m_szDirectory, files[index].c_str());

			if (length >= 0 && (unsigned int)length < sizeof(path))
				g_pFileSystem->RemoveFile(path, "GAMECONFIG");

			files.erase(files.begin() + index);
		}
	}

	return files.size();
}
