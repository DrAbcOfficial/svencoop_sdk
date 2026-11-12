# 32-bit x86 Linux toolchain for the Sven Co-op SDK.
#
# GoldSrc and its server dependencies are 32-bit, so the module has to be
# built with -m32.
#
# Usage:
#   cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-linux-x86.cmake

# Force the x86 target name.  Without this, building on a non-x86 host (for
# example an AArch64 build machine with an i386 multilib cross toolchain)
# would report the host CPU, and the AngelScript / Crypto++ source and flag
# selection would be wrong.
set(CMAKE_SYSTEM_PROCESSOR "i686" CACHE STRING "" FORCE)

set(CMAKE_C_FLAGS_INIT   "-m32")
set(CMAKE_CXX_FLAGS_INIT "-m32")
set(CMAKE_ASM_FLAGS_INIT "-m32")
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
