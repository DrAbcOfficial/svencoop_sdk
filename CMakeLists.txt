# Sven Co-op SDK - dedicated server CMake build
#
# Supported targets:
#   * server.dll  (Windows, Win32; MSVC or clang-cl)
#   * hl_i386.so  (Linux x86, 32-bit; GCC or Clang)
#   * server.so   (Linux x86_64/aarch64; -DSVENCOOP_NATIVE_64BIT=ON)
#   * server.dll  (Windows x64/ARM64; -DSVENCOOP_NATIVE_64BIT=ON)
#
# The GoldSrc engine and the bundled server-side third-party dependencies are
# 32-bit.  A native 64-bit build is only meant as a compiler/linker syntax
# check; the resulting module cannot be loaded by the 32-bit engine.

cmake_minimum_required(VERSION 3.20)

project(SvenCoopSDK VERSION 5.26 LANGUAGES C CXX)

# The project is pinned to C++20: no C++23, no GNU dialect.  GCC/Clang get
# -std=c++20 (CMAKE_CXX_EXTENSIONS OFF); MSVC gets /std:c++20 plus
# /permissive- below.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Toolchain requirements (strict C++20)
#
# The server uses C++20 library facilities directly (std::jthread,
# std::stop_token) and is built without GNU dialect extensions, so the
# toolchain must implement them.  Fail at configure time with an actionable
# message instead of emitting hundreds of compile errors.
# ---------------------------------------------------------------------------
include(CheckCXXSourceCompiles)

# The feature test below links a real std::jthread, so POSIX toolchains need
# the thread library on the link line as well (older glibc keeps pthread
# outside libc).
if(NOT MSVC)
    set(CMAKE_REQUIRED_FLAGS "-pthread")
endif()

check_cxx_source_compiles("
    #include <thread>
    #include <stop_token>
    int main()
    {
        std::stop_source source;
        std::jthread thread( []( std::stop_token token ) { (void) token; } );
        (void) source.get_token();
        return 0;
    }
" SVENCOOP_HAVE_CXX20_JTHREAD)

unset(CMAKE_REQUIRED_FLAGS)

if(NOT SVENCOOP_HAVE_CXX20_JTHREAD)
    message(FATAL_ERROR
        "This project requires full C++20 thread support (std::jthread, "
        "std::stop_token).\n"
        "Upgrade the toolchain: GCC 10+, Clang with a C++20 standard library "
        "(libstdc++ 10+ or libc++ with <stop_token>), or MSVC 19.28+ "
        "(Visual Studio 2019 16.8 or newer).")
endif()

# MSVC and clang-cl both report MSVC=TRUE (CMake treats clang-cl as a
# compiler simulating the MSVC command line), so the MSVC branch below
# covers both.  Everything else uses the GNU-style command line.
# NOTE: the probe variable must not be pre-set: check_cxx_source_compiles()
# skips the test when its result variable is already defined, which would
# silently drop the definition on every non-GCC compiler.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SVENCOOP_USES_LIBSTDCXX TRUE)
elseif(MSVC OR APPLE)
    set(SVENCOOP_USES_LIBSTDCXX FALSE)
else()
    check_cxx_source_compiles("
        #include <string>
        #ifndef _GLIBCXX_USE_CXX11_ABI
        #error libstdc++ only
        #endif
        int main() { return 0; }
    " SVENCOOP_HAVE_LIBSTDCXX_ABI_MACRO)

    if(SVENCOOP_HAVE_LIBSTDCXX_ABI_MACRO)
        set(SVENCOOP_USES_LIBSTDCXX TRUE)
    else()
        set(SVENCOOP_USES_LIBSTDCXX FALSE)
    endif()
endif()

message(STATUS "Standard library: libstdc++ (old ABI): ${SVENCOOP_USES_LIBSTDCXX}")

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

option(SVENCOOP_WARNINGS     "Enable the compiler warning level used by the old projects" ON)
option(SVENCOOP_PEDANTIC     "Enable -Wpedantic (accurate but noisy on this legacy code base)" ON)
option(SVENCOOP_ALLOW_64BIT  "Allow a 64-bit Linux build (requires 64-bit dependencies)" OFF)
option(SVENCOOP_NATIVE_64BIT "Build a native 64-bit module (Linux x86_64/aarch64, Windows x64/ARM64) for syntax validation only; it is not loadable by the 32-bit engine" OFF)

# ---------------------------------------------------------------------------
# Architecture checks
# ---------------------------------------------------------------------------
set(SVENCOOP_TARGET_64BIT FALSE)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SVENCOOP_TARGET_64BIT TRUE)
endif()

# Target CPU family; used for the x86-only SIMD compile options and for
# picking the AngelScript native calling convention sources.
#
# CMAKE_SYSTEM_PROCESSOR may report the host CPU (Visual Studio -A Win32,
# cross toolchains), so the pointer size decides between the 32- and 64-bit
# name of the same family.
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SVENCOOP_SYSTEM_PROCESSOR_LOWER)

# The Visual Studio generator reports the *host* processor in
# CMAKE_SYSTEM_PROCESSOR (an x64 host stays "AMD64" for -A Win32), so the
# generator platform is the authoritative target name there.
if(WIN32 AND CMAKE_GENERATOR_PLATFORM)
    string(TOLOWER "${CMAKE_GENERATOR_PLATFORM}" SVENCOOP_GENERATOR_PLATFORM_LOWER)
    if(SVENCOOP_GENERATOR_PLATFORM_LOWER STREQUAL "win32"
            OR SVENCOOP_GENERATOR_PLATFORM_LOWER STREQUAL "x86")
        set(SVENCOOP_SYSTEM_PROCESSOR_LOWER "x86")
    elseif(SVENCOOP_GENERATOR_PLATFORM_LOWER STREQUAL "x64"
            OR SVENCOOP_GENERATOR_PLATFORM_LOWER STREQUAL "amd64")
        set(SVENCOOP_SYSTEM_PROCESSOR_LOWER "x86_64")
    elseif(SVENCOOP_GENERATOR_PLATFORM_LOWER MATCHES "^arm64")
        set(SVENCOOP_SYSTEM_PROCESSOR_LOWER "arm64")
    endif()
endif()

set(SVENCOOP_TARGET_ARCH "unknown")
if(SVENCOOP_TARGET_64BIT)
    if(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(x86_64|amd64|x64)")
        set(SVENCOOP_TARGET_ARCH "x86_64")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(aarch64|arm64)")
        set(SVENCOOP_TARGET_ARCH "arm64")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(ppc64|ppc64le)")
        set(SVENCOOP_TARGET_ARCH "ppc64")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(mips64)")
        set(SVENCOOP_TARGET_ARCH "mips64")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(riscv64)")
        set(SVENCOOP_TARGET_ARCH "riscv64")
    endif()
else()
    if(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(i[3-6]86|x86|ia32|win32)")
        set(SVENCOOP_TARGET_ARCH "x86")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(arm|armv[0-9].*)")
        set(SVENCOOP_TARGET_ARCH "arm")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(mips)")
        set(SVENCOOP_TARGET_ARCH "mips")
    elseif(SVENCOOP_SYSTEM_PROCESSOR_LOWER MATCHES "^(ppc)")
        set(SVENCOOP_TARGET_ARCH "ppc")
    endif()
endif()

if(WIN32 AND SVENCOOP_TARGET_64BIT AND NOT SVENCOOP_NATIVE_64BIT)
    message(FATAL_ERROR
        "This project is 32-bit only. Configure the Visual Studio generator with "
        "'-A Win32' (e.g. cmake -S . -B build -G \"Visual Studio 17 2022\" -A Win32), "
        "or pass -DSVENCOOP_NATIVE_64BIT=ON for a 64-bit syntax-validation build.")
endif()

if(UNIX AND NOT APPLE AND SVENCOOP_TARGET_64BIT
        AND NOT SVENCOOP_ALLOW_64BIT AND NOT SVENCOOP_NATIVE_64BIT)
    message(FATAL_ERROR
        "GoldSrc and the bundled dependencies are 32-bit. Either configure with the "
        "supplied toolchain file: "
        "cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-linux-x86.cmake "
        "(needs 32-bit multilib), or pass -DSVENCOOP_NATIVE_64BIT=ON for a native "
        "64-bit build that only validates the syntax and cannot run.")
endif()

if(SVENCOOP_TARGET_64BIT)
    message(STATUS "Target: 64-bit (${SVENCOOP_TARGET_ARCH}); syntax-validation build")
else()
    message(STATUS "Target: 32-bit (${SVENCOOP_TARGET_ARCH})")
endif()

# Assembler languages must be enabled at directory scope; enabling them from
# inside a function leaves CMAKE_ASM_COMPILE_OBJECT unset at generate time.
if(SVENCOOP_TARGET_ARCH STREQUAL "arm64" OR SVENCOOP_TARGET_ARCH STREQUAL "arm")
    if(MSVC)
        # armasm/armasm64, not the x86 ml/ml64 dialect.
        if(CMAKE_VERSION VERSION_LESS 3.26)
            message(FATAL_ERROR
                "MSVC ARM/ARM64 builds require CMake 3.26 or newer for the "
                "ASM_MARMASM language.")
        endif()
        enable_language(ASM_MARMASM)
    else()
        enable_language(ASM)
    endif()
elseif(SVENCOOP_TARGET_ARCH STREQUAL "x86_64" AND WIN32 AND MSVC)
    enable_language(ASM_MASM)
endif()

# ---------------------------------------------------------------------------
# Global compiler configuration
# ---------------------------------------------------------------------------
if(MSVC)
    # The original projects used the static CRT (/MT, /MTd).
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _ITERATOR_DEBUG_LEVEL=0)

    # Strict C++20: no Microsoft language extensions, standards-conforming
    # name lookup, and a correct __cplusplus value.  /utf-8 is required
    # because several legacy headers used to contain Latin-1 bytes and the
    # default source encoding is the system code page (GBK on Chinese
    # Windows).
    # Keep C/C++-only switches away from MASM/armasm, which reject them.
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/utf-8>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/permissive->)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>)
    # /permissive- implies /Zc:strictStrings.  That diagnostic rejects
    # hundreds of legacy `char *` initializers and engine callback
    # parameters (cvar_t tables, ALERT formats) which cannot be changed
    # without breaking the engine ABI, so string-literal strictness stays
    # off while the rest of /permissive- is kept.
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Zc:strictStrings->)
    add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/MP>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Zc:threadSafeInit->)
    if(SVENCOOP_WARNINGS)
        add_compile_options($<$<COMPILE_LANGUAGE:C,CXX>:/W3>)
    endif()
else()
    # POSIX/GCC definitions shared by the server.
    add_compile_definitions(
        NO_MALLOC_OVERRIDE
        _alloca=alloca
    )
    # The old libstdc++ ABI is used for binary compatibility with the
    # engine-side toolchain.  It is a libstdc++ knob: never pass it when
    # another standard library (libc++) is in use.
    if(SVENCOOP_USES_LIBSTDCXX)
        add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
    endif()
    add_compile_options(-fno-strict-aliasing)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # -fno-gnu-unique keeps dlclose()/reload behaviour sane on Linux.
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            add_compile_options(-fno-gnu-unique)
        endif()
    endif()
    # -fpermissive was inherited from the old Makefiles.  Under strict C++20
    # it is not used: the tree compiles cleanly without it.
    if(SVENCOOP_WARNINGS)
        add_compile_options(-Werror=return-type)
        if(SVENCOOP_PEDANTIC)
            add_compile_options(-Wpedantic)
        endif()
    endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/include/angelscript.h")
    message(FATAL_ERROR
        "The AngelScript submodule is missing. Run:\n"
        "  git submodule update --init --recursive")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/sources.cmake)

# ---------------------------------------------------------------------------
# Per-source tweaks
# ---------------------------------------------------------------------------

# AngelScript's scriptmath add-on is built with floats in this SDK.
set_source_files_properties(
    external/angelscript/sdk/add_on/scriptmath/scriptmath.cpp
    PROPERTIES COMPILE_DEFINITIONS "AS_USE_FLOAT=1"
)

# Crypto++ 8.9.0 still uses stdext::make_checked_array_iterator, which was
# removed from newer MSVC standard libraries. Probe for it and supply a small
# compatibility shim only when necessary.
if(MSVC)
    include(CheckCXXSourceCompiles)
    check_cxx_source_compiles("
        #include <iterator>
        int main()
        {
            unsigned char buffer[1] = { 0 };
            auto it = stdext::make_checked_array_iterator(buffer, 1);
            (void)it;
            return 0;
        }
    " SVENCOOP_HAVE_STDEXT_CHECKED_ARRAY_ITERATOR)

    if(NOT SVENCOOP_HAVE_STDEXT_CHECKED_ARRAY_ITERATOR)
        set_source_files_properties(external/cryptopp/integer.cpp
            PROPERTIES COMPILE_OPTIONS
            "/FI${CMAKE_CURRENT_SOURCE_DIR}/cmake/compat/msvc_checked_iterator.h")
    endif()
endif()

# Crypto++ selects its SIMD implementations at compile time on GCC/Clang.
# The explicit instruction-set flags below are per-architecture: x86 (both
# 32- and 64-bit) needs its extension flags, AArch64 needs the crypto
# extension for the ARMv8 paths (Crypto++ enables them optimistically for
# GCC >= 4.8).
if(NOT MSVC)
    if(SVENCOOP_TARGET_ARCH STREQUAL "x86" OR SVENCOOP_TARGET_ARCH STREQUAL "x86_64")
        set_source_files_properties(external/cryptopp/gcm_simd.cpp      PROPERTIES COMPILE_OPTIONS "-mssse3;-mpclmul")
        set_source_files_properties(external/cryptopp/rijndael_simd.cpp PROPERTIES COMPILE_OPTIONS "-msse4.1;-maes")
        set_source_files_properties(external/cryptopp/sha_simd.cpp      PROPERTIES COMPILE_OPTIONS "-msse4.2;-msha")
        set_source_files_properties(external/cryptopp/sse_simd.cpp      PROPERTIES COMPILE_OPTIONS "-msse2")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "arm64")
        set_source_files_properties(external/cryptopp/gcm_simd.cpp      PROPERTIES COMPILE_OPTIONS "-march=armv8-a+crypto")
        set_source_files_properties(external/cryptopp/rijndael_simd.cpp PROPERTIES COMPILE_OPTIONS "-march=armv8-a+crypto")
        set_source_files_properties(external/cryptopp/sha_simd.cpp      PROPERTIES COMPILE_OPTIONS "-march=armv8-a+crypto")
        set_source_files_properties(external/cryptopp/sse_simd.cpp      PROPERTIES COMPILE_OPTIONS "-march=armv8-a+crypto")
    endif()
endif()

# ---------------------------------------------------------------------------
# Include directories / definitions
# ---------------------------------------------------------------------------
set(SVENCOOP_COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/dlls
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/pm_shared
    ${CMAKE_CURRENT_SOURCE_DIR}/game_shared
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

set(SVENCOOP_COMMON_DEFINITIONS
    QUIVER
    VOXEL
    QUAKE2
)

if(WIN32)
    # WIN32/_WINDOWS are supplied by the compiler/CMake.
elseif(APPLE)
    list(APPEND SVENCOOP_COMMON_DEFINITIONS POSIX _POSIX GNUC HL1 OSX _OSX)
    list(APPEND SVENCOOP_COMMON_DEFINITIONS
        "stricmp=strcasecmp" "_strnicmp=strncasecmp" "strnicmp=strncasecmp")
else()
    list(APPEND SVENCOOP_COMMON_DEFINITIONS POSIX _POSIX GNUC HL1 linux LINUX _LINUX)
    list(APPEND SVENCOOP_COMMON_DEFINITIONS
        "stricmp=strcasecmp" "_strnicmp=strncasecmp" "strnicmp=strncasecmp")
endif()

# ---------------------------------------------------------------------------
# libcurl import library (Windows)
#
# Windows is supported with MSVC/clang-cl only (MinGW is out of scope), so the
# import library is generated with lib.exe.  The machine type follows the
# target architecture, not the host.
# ---------------------------------------------------------------------------
if(WIN32 AND NOT MSVC)
    message(FATAL_ERROR
        "Windows builds require MSVC or clang-cl. MinGW and other "
        "GNU-compatible Windows toolchains are not supported.")
endif()

if(WIN32 AND MSVC)
    get_filename_component(SVENCOOP_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_program(SVENCOOP_LIB_EXE
        NAMES lib.exe
        HINTS "${SVENCOOP_MSVC_BIN_DIR}"
        NO_DEFAULT_PATH)
    if(NOT SVENCOOP_LIB_EXE)
        find_program(SVENCOOP_LIB_EXE NAMES lib.exe)
    endif()

    set(SVENCOOP_CURL_DEF "${CMAKE_CURRENT_SOURCE_DIR}/external/curl-win32/libcurl_sven.def")
    set(SVENCOOP_CURL_LIB "${CMAKE_CURRENT_BINARY_DIR}/libcurl_sven.lib")

    if(SVENCOOP_TARGET_ARCH STREQUAL "x86")
        set(SVENCOOP_LIB_MACHINE "x86")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "x86_64")
        set(SVENCOOP_LIB_MACHINE "x64")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "arm64")
        set(SVENCOOP_LIB_MACHINE "arm64")
    else()
        set(SVENCOOP_LIB_MACHINE "x86")
    endif()

    if(SVENCOOP_LIB_EXE)
        add_custom_command(
            OUTPUT "${SVENCOOP_CURL_LIB}"
            COMMAND "${SVENCOOP_LIB_EXE}" /nologo /machine:${SVENCOOP_LIB_MACHINE}
                    /def:${SVENCOOP_CURL_DEF} /out:${SVENCOOP_CURL_LIB}
            DEPENDS "${SVENCOOP_CURL_DEF}"
            COMMENT "Generating libcurl_sven.lib"
            VERBATIM)
        add_custom_target(svencoop_curl_import ALL DEPENDS "${SVENCOOP_CURL_LIB}")
    else()
        message(WARNING "lib.exe was not found; the server cannot be linked against libcurl_sven.def")
    endif()
endif()

# ---------------------------------------------------------------------------
# miniz (official CMake project)
# ---------------------------------------------------------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_FUZZERS  OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(INSTALL_PROJECT OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(external/miniz EXCLUDE_FROM_ALL)

# ---------------------------------------------------------------------------
# Output directories
# ---------------------------------------------------------------------------
set(SVENCOOP_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(SVENCOOP_LIBRARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(SVENCOOP_ARCHIVE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")

# ---------------------------------------------------------------------------
# Server module
# ---------------------------------------------------------------------------
add_library(svencoop_server_runtime STATIC
    ${SVENCOOP_SERVER_RUNTIME_SOURCES}
)

# Crypto++ implements the x64 CPUID/XGETBV helpers and several SIMD kernels
# (integer add/sub, AES-NI, CLMUL, SHA) in MASM for MSVC.  They are not part
# of the portable C++ source list, so an x64 MSVC build must add the module
# explicitly or the server fails to link.
if(MSVC AND SVENCOOP_TARGET_ARCH STREQUAL "x86_64")
    target_sources(svencoop_server_runtime PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/cryptopp/x64dll.asm)
endif()
target_include_directories(svencoop_server_runtime PRIVATE
    ${SVENCOOP_COMMON_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
)
target_compile_definitions(svencoop_server_runtime PRIVATE
    ${SVENCOOP_COMMON_DEFINITIONS}
    VALVE_DLL
    CLIENT_WEAPONS
    AS_PROCESS_METADATA=0
    AS_USE_STLNAMES=1
    AS_USE_ACCESSORS=0
)
if(NOT WIN32)
    # pm_shared/pm_math.cpp uses the engine's vec3_t layout on non-Windows.
    target_compile_definitions(svencoop_server_runtime PRIVATE DISABLE_VEC_ORIGIN)
endif()
if(NOT MSVC)
    target_compile_options(svencoop_server_runtime PRIVATE -fexceptions)
endif()

add_library(svencoop_server MODULE
    ${SVENCOOP_SERVER_SOURCES}
)

target_include_directories(svencoop_server PRIVATE
    ${SVENCOOP_COMMON_INCLUDES}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/cryptopp
)

# Crypto++ needs manual template instantiation in this codebase.
target_sources(svencoop_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/compat/cryptopp_client_templates.cpp
)

# ---------------------------------------------------------------------------
# AngelScript native calling convention
#
# AngelScript picks AS_X86 / AS_X64_GCC / AS_X64_MSVC / AS_ARM / AS_ARM64 in
# as_config.h and needs the matching out-of-line trampoline sources.  A source
# compiled for a convention the library did not select is harmless (it
# compiles to nothing), but a missing one breaks the link.  Unknown CPUs fall
# back to AS_MAX_PORTABILITY and need nothing extra.
#
# Supported targets: Windows (MSVC / clang-cl) x86 and x64, Linux/macOS
# x86_64, Linux/Windows/macOS ARM64.
# ---------------------------------------------------------------------------
function(svencoop_add_angelscript_callfunc target)
    set(_as "${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/source")

    if(SVENCOOP_TARGET_ARCH STREQUAL "x86")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_x86.cpp")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "x86_64")
        if(WIN32)
            if(MSVC)
                target_sources(${target} PRIVATE
                    "${_as}/as_callfunc_x64_msvc.cpp"
                    "${_as}/as_callfunc_x64_msvc_asm.asm")
            else()
                message(FATAL_ERROR "Windows x64 builds require MSVC or clang-cl.")
            endif()
        else()
            target_sources(${target} PRIVATE "${_as}/as_callfunc_x64_gcc.cpp")
        endif()
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "arm64")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_arm64.cpp")
        if(MSVC)
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm64_msvc.asm")
        elseif(APPLE)
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm64_xcode.S")
        else()
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm64_gcc.S")
        endif()
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "arm")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_arm.cpp")
        if(MSVC)
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm_msvc.asm")
        elseif(APPLE)
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm_xcode.S")
        else()
            target_sources(${target} PRIVATE "${_as}/as_callfunc_arm_gcc.S")
        endif()
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "ppc64")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_ppc_64.cpp")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "ppc")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_ppc.cpp")
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "mips")
        target_sources(${target} PRIVATE "${_as}/as_callfunc_mips.cpp")
    else()
        message(STATUS
            "AngelScript: no native calling convention for '${SVENCOOP_TARGET_ARCH}'; "
            "using the portable AS_MAX_PORTABILITY implementation")
    endif()
endfunction()

svencoop_add_angelscript_callfunc(svencoop_server)
target_compile_definitions(svencoop_server PRIVATE
    ${SVENCOOP_COMMON_DEFINITIONS}
    VALVE_DLL
    CLIENT_WEAPONS
    AS_PROCESS_METADATA=0
    AS_USE_STLNAMES=1
    AS_USE_ACCESSORS=0
)

target_link_libraries(svencoop_server PRIVATE svencoop_server_runtime miniz)

if(WIN32)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dlls/hl.def")
        target_sources(svencoop_server PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/dlls/hl.def")
    endif()
    # Winsock 2 supersedes the legacy wsock32 import library and is available
    # for every supported Windows architecture.
    target_link_libraries(svencoop_server PRIVATE ws2_32)
    if(SVENCOOP_CURL_LIB)
        target_link_libraries(svencoop_server PRIVATE "${SVENCOOP_CURL_LIB}")
        if(TARGET svencoop_curl_import)
            add_dependencies(svencoop_server svencoop_curl_import)
        endif()
    endif()
    # The MSVC CRT helper object referenced by the original project.  It is an
    # x86-only implementation detail of the MSVC toolset.
    if(SVENCOOP_TARGET_ARCH STREQUAL "x86")
        get_filename_component(SVENCOOP_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        get_filename_component(SVENCOOP_MSVC_TOOLSET_DIR "${SVENCOOP_MSVC_BIN_DIR}/../../.." ABSOLUTE)
        set(SVENCOOP_FLOAT_ROUNDING_OBJ "${SVENCOOP_MSVC_TOOLSET_DIR}/lib/x86/legacy_stdio_float_rounding.obj")
        if(EXISTS "${SVENCOOP_FLOAT_ROUNDING_OBJ}")
            target_link_options(svencoop_server PRIVATE "${SVENCOOP_FLOAT_ROUNDING_OBJ}")
        endif()
    endif()
    set_target_properties(svencoop_server PROPERTIES OUTPUT_NAME "server")
else()
    target_compile_definitions(svencoop_server PRIVATE DISABLE_VEC_ORIGIN)
    if(NOT MSVC)
        target_compile_options(svencoop_server PRIVATE -fexceptions)
    endif()

    # CPlayerDatabase uses the system libcurl on non-Windows platforms.
    find_package(CURL REQUIRED)
    find_package(Threads REQUIRED)

    target_link_libraries(svencoop_server PRIVATE CURL::libcurl Threads::Threads)
    # CMAKE_DL_LIBS is "dl" on Linux and empty where the loader is part of
    # libc (macOS, BSD).  libm is likewise folded into libSystem on macOS.
    if(CMAKE_DL_LIBS)
        target_link_libraries(svencoop_server PRIVATE ${CMAKE_DL_LIBS})
    endif()
    if(NOT APPLE)
        target_link_libraries(svencoop_server PRIVATE m)
    endif()

    if(APPLE)
        set_target_properties(svencoop_server PROPERTIES
            PREFIX ""
            OUTPUT_NAME "server"
            SUFFIX ".dylib")
    elseif(SVENCOOP_TARGET_64BIT)
        # Native 64-bit build: name it like the Windows module.
        set_target_properties(svencoop_server PROPERTIES
            PREFIX ""
            OUTPUT_NAME "server")
    else()
        # Keep the file names produced by the former Linux Makefiles.
        set_target_properties(svencoop_server PROPERTIES
            PREFIX ""
            OUTPUT_NAME "hl_i386")
    endif()
endif()

set_target_properties(svencoop_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SVENCOOP_RUNTIME_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SVENCOOP_LIBRARY_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${SVENCOOP_ARCHIVE_OUTPUT_DIR}"
)
