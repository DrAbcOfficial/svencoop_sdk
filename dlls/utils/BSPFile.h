#ifndef BSPFILE_H
#define BSPFILE_H

#include "utils/CMemory.h"

struct dheader_s;

dheader_s *LoadBSPFile(const char *fileName, CMemory<unsigned int> &memory);

#endif
