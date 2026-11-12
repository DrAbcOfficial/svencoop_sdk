#ifndef CSTRINGHASH_H
#define CSTRINGHASH_H

#include "CString.h"
#include <functional>
#include <string.h>

namespace std
{
	template<>
	struct hash<CString>
	{
		size_t operator()(const CString &text) const
		{
			const unsigned int OFFSET_BASIS = 2166136261u;
			const unsigned int PRIME = 16777619u;
			const unsigned int SAMPLE_DIVISOR = 10;
			const char *data = text.Data();
			unsigned int result = OFFSET_BASIS;

			if (data)
			{
				unsigned int length = strlen(data);
				unsigned int step = length / SAMPLE_DIVISOR + 1;

				for (unsigned int i = 0; i < length; i += step)
					result = result * PRIME ^ (signed char)data[i];
			}

			return result;
		}
	};
}

#endif
