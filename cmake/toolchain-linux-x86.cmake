# 32-bit x86 Linux toolchain for the Sven Co-op SDK.
#
# GoldSrc and the bundled dependencies (vgui, SDL2, FMOD Ex, Discord RPC) are
# 32-bit, so the modules have to be built with -m32.
#
# Usage:
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-linux-x86.cmake

set(CMAKE_C_FLAGS_INIT   "-m32")
set(CMAKE_CXX_FLAGS_INIT "-m32")
set(CMAKE_EXE_LINKER_FLAGS_INIT    "-m32")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-m32")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-m32")

# Help find_library()/find_package() pick the multilib (i386) copies instead of
# the host's 64-bit libraries.
list(APPEND CMAKE_LIBRARY_PATH
    /usr/lib/i386-linux-gnu
    /usr/lib32
    /lib/i386-linux-gnu
    /lib32)
