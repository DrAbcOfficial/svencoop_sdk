#ifndef BSPFILE_H
#define BSPFILE_H

#include <vector>

struct dheader_s;

dheader_s *LoadBSPFile(const char *fileName, std::vector<char> &memory);

#endif
