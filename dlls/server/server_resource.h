#ifndef SERVER_RESOURCE_H
#define SERVER_RESOURCE_H

bool UTIL_ExtractDirectoryFromFilename(const char *filename, char *directory, unsigned int size, bool stripLeadingSlashes);
char *UTIL_FindAbstractResourcePath(const char *filename, char *path, unsigned int size);

#endif
