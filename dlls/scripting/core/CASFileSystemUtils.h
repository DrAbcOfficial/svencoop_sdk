#ifndef CASFILESYSTEMUTILS_H
#define CASFILESYSTEMUTILS_H

typedef unsigned char OpenFileFlags_t;
typedef unsigned char SeekFileFlags_t;

class CString;

namespace OpenFileFlagBit
{
	enum
	{
		READ = 1 << 0,
		WRITE = 1 << 1,
		APPEND = 1 << 2,
		BINARY = 1 << 3
	};
}

namespace SeekFileFlagBit
{
	enum
	{
		SET = 0,
		CUR = 1,
		END = 2
	};
}

class CASFileSystemUtils
{
public:
	static OpenFileFlags_t FilterFlags(OpenFileFlags_t flags);
	static bool FlagsValid(OpenFileFlags_t flags);
	static bool FormatOpenFlags(OpenFileFlags_t flags, CString &mode);
	static void CreateDirectory(const char *directory);
	static void RemoveFile(const char *fileName);
};

#endif
