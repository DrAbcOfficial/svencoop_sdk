#include "extdll.h"
#include "scripting/core/CASFileQuota.h"
#include "scripting/core/CASLog.h"
#include "server/server_filesystem.h"

CASFileQuota::CASFileQuota(unsigned int totalSize, unsigned int fileLimit)
	: m_uiTotalSize(totalSize), m_uiSizeInUse(0), m_uiFileLimit(fileLimit)
{
}

CASFileQuota::~CASFileQuota()
{
}

void CASFileQuota::Release(void) const
{
	if (InternalRelease())
		delete this;
}

unsigned int CASFileQuota::GetTotalSize(void) const
{
	return m_uiTotalSize;
}

unsigned int CASFileQuota::GetSizeInUse(void) const
{
	return m_uiSizeInUse;
}

unsigned int CASFileQuota::GetSizeLeft(void) const
{
	return m_uiTotalSize - m_uiSizeInUse;
}

bool CASFileQuota::CanWriteAmount(unsigned int size) const
{
	return size <= m_uiTotalSize - m_uiSizeInUse;
}

unsigned int CASFileQuota::SizeLeftAfterWrite(unsigned int size) const
{
	return CanWriteAmount(size) ? m_uiTotalSize - m_uiSizeInUse - size : 0;
}

void CASFileQuota::BytesWritten(unsigned int size)
{
	if (m_uiSizeInUse <= m_uiTotalSize - size)
		m_uiSizeInUse += size;
	else
		m_uiSizeInUse = m_uiTotalSize;
}

void CASFileQuota::BytesRemoved(unsigned int size)
{
	if (size <= m_uiSizeInUse)
		m_uiSizeInUse -= size;
	else
		m_uiSizeInUse = 0;
}

bool CASFileQuota::FileLimitReached(void) const
{
	return m_Files.size() >= m_uiFileLimit;
}

unsigned int CASFileQuota::GetBytesWrittenToFile(const CString &fileName) const
{
	FileMap_t::const_iterator it = m_Files.find(fileName);

	return it != m_Files.end() ? it->second.m_uiBytesWritten : 0;
}

void CASFileQuota::AddWriteFileToList(const CString &fileName, OpenFileFlags_t flags)
{
	FileMap_t::iterator it = m_Files.find(fileName);

	if (it == m_Files.end())
	{
		FileData_t data;

		data.m_szFileName = fileName;
		data.m_uiBytesWritten = 0;
		m_Files.insert(std::make_pair(fileName, data));
	}
	else if (flags & OpenFileFlagBit::WRITE)
	{
		BytesRemoved(it->second.m_uiBytesWritten);
		it->second.m_uiBytesWritten = 0;
	}
}

void CASFileQuota::UpdateBytesWrittenToFile(const CString &fileName, unsigned int size)
{
	FileMap_t::iterator it = m_Files.find(fileName);

	if (it == m_Files.end())
	{
		CASLog::GetInstance()->Error(AS_LOG_DEFAULT, "CASFileQuota::UpdateBytesWrittenToFile: file was not in the list!\n");
		return;
	}

	m_uiSizeInUse += size - it->second.m_uiBytesWritten;
	it->second.m_uiBytesWritten = size;
}

void CASFileQuota::RemoveWriteFileFromList(const CString &fileName)
{
	FileMap_t::iterator it = m_Files.find(fileName);

	if (it != m_Files.end())
	{
		m_uiSizeInUse -= it->second.m_uiBytesWritten;
		m_Files.erase(it);
	}
}

void CASFileQuota::RemoveFilesInDirectory(const char *directory)
{
	if (!directory)
		return;

	if (!strncmp(directory, "./", sizeof("./") - 1))
		directory += sizeof("./") - 1;

	if (!*directory)
		return;

	unsigned int length = strlen(directory);
	FileMap_t::iterator it = m_Files.begin();

	while (it != m_Files.end())
	{
		if (strncmp(it->first.Data(), directory, length))
		{
			it++;
			continue;
		}

		CASFileSystemUtils::RemoveFile(it->first.Data());
		m_uiSizeInUse -= it->second.m_uiBytesWritten;
		m_Files.erase(it++);
	}
}

void CASFileQuota::RemoveAllFiles(void)
{
	m_Files.clear();
}

void CASFileQuota::ValidateFileList(void)
{
	FileMap_t::iterator it = m_Files.begin();

	while (it != m_Files.end())
	{
		FileHandle_t file = g_pFileSystem->Open(it->first.Data(), "r", "GAME");

		if (file)
		{
			g_pFileSystem->Close(file);
			it++;
		}
		else
		{
			m_uiSizeInUse -= it->second.m_uiBytesWritten;
			m_Files.erase(it++);
		}
	}
}
