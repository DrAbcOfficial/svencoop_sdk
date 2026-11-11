// Compatibility shim for Crypto++ 8.9.0 on MSVC versions where
// stdext::make_checked_array_iterator has been removed from <iterator>.
// The CMake build only force-includes this header when a configure-time
// check proves that the real function is unavailable.
#pragma once

#include <cstddef>

#if defined(_MSC_VER)
namespace stdext
{
template <typename Iter>
inline Iter make_checked_array_iterator(Iter it, std::size_t)
{
    return it;
}
}
#endif
