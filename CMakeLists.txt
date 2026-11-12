# Sven Co-op SDK - dedicated server CMake build
#
# Builds only the server module:
#   * server.dll  (Windows, 32-bit)
#   * hl_i386.so  (Linux, 32-bit)
#   * server.so   (Linux, native 64-bit; SVENCOOP_NATIVE_64BIT=ON)
#
# The GoldSrc engine and the bundled server-side third-party dependencies are
# 32-bit x86. A native 64-bit Linux build is only meant as a syntax check by
# the compiler; the resulting module cannot be loaded by the 32-bit engine.

cmake_minimum_required(VERSION 3.20)

project(SvenCoopSDK VERSION 5.26 LANGUAGES C CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SVENCOOP_WARNINGS     "Enable the compiler warning level used by the old projects" ON)
option(SVENCOOP_ALLOW_64BIT  "Allow a 64-bit Linux build (requires 64-bit dependencies)" OFF)
option(SVENCOOP_NATIVE_64BIT "Build a native 64-bit Linux module (x86_64/aarch64) for syntax validation only; it is not loadable by the 32-bit engine" OFF)

# ---------------------------------------------------------------------------
# Architecture checks
# ---------------------------------------------------------------------------
set(SVENCOOP_TARGET_64BIT FALSE)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(SVENCOOP_TARGET_64BIT TRUE)
endif()

# Target CPU family; used for the x86-only SIMD compile options and for
# picking the AngelScript native calling convention sources.
set(SVENCOOP_TARGET_ARCH "unknown")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86|X86|AMD64|amd64|i[3-6]86|x86_64|X86_64)")
    set(SVENCOOP_TARGET_ARCH "x86")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(arm64|ARM64|aarch64|AARCH64)")
    set(SVENCOOP_TARGET_ARCH "arm64")
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

# ---------------------------------------------------------------------------
# Global compiler configuration
# ---------------------------------------------------------------------------
if(MSVC)
    # The original projects used the static CRT (/MT, /MTd).
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _ITERATOR_DEBUG_LEVEL=0)
    add_compile_options(/MP)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Zc:threadSafeInit->)
    if(SVENCOOP_WARNINGS)
        add_compile_options(/W3)
    endif()
else()
    # POSIX/GCC definitions shared by the server.
    add_compile_definitions(
        _GLIBCXX_USE_CXX11_ABI=0
        NO_MALLOC_OVERRIDE
        _alloca=alloca
    )
    add_compile_options(-fno-strict-aliasing)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fpermissive>)
        if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
            add_compile_options(-fno-gnu-unique)
        endif()
    endif()
    if(SVENCOOP_WARNINGS)
        add_compile_options(-Werror=return-type)
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
# The explicit instruction-set flags below are per-architecture: x86 needs its
# extension flags, AArch64 needs the crypto extension for the ARMv8 paths
# (Crypto++ enables them optimistically for GCC >= 4.8).
if(NOT MSVC)
    if(SVENCOOP_TARGET_ARCH STREQUAL "x86")
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
# The server only ships libcurl_sven.def; the original project generated the
# import library with lib.exe before linking.
# ---------------------------------------------------------------------------
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

    if(SVENCOOP_LIB_EXE)
        add_custom_command(
            OUTPUT "${SVENCOOP_CURL_LIB}"
            COMMAND "${SVENCOOP_LIB_EXE}" /nologo /machine:x86
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
    target_compile_definitions(svencoop_server_runtime PRIVATE DISABLE_VEC_ORIGIN)
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

# On a 64-bit target AngelScript auto-selects a different native calling
# convention (AS_X64_GCC / AS_ARM64), which needs its matching callfunc
# sources. Unknown architectures fall back to AS_MAX_PORTABILITY and use the
# portable implementation already present in as_callfunc.cpp.
if(UNIX AND NOT APPLE AND SVENCOOP_TARGET_64BIT)
    if(SVENCOOP_TARGET_ARCH STREQUAL "arm64")
        enable_language(ASM)
        target_sources(svencoop_server PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/source/as_callfunc_arm64.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/source/as_callfunc_arm64_gcc.S
        )
    elseif(SVENCOOP_TARGET_ARCH STREQUAL "x86")
        target_sources(svencoop_server PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/external/angelscript/sdk/angelscript/source/as_callfunc_x64_gcc.cpp
        )
    endif()
endif()
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
    target_link_libraries(svencoop_server PRIVATE wsock32)
    if(SVENCOOP_CURL_LIB AND SVENCOOP_LIB_EXE)
        target_link_libraries(svencoop_server PRIVATE "${SVENCOOP_CURL_LIB}")
        add_dependencies(svencoop_server svencoop_curl_import)
    endif()
    # The MSVC CRT helper object referenced by the original project.
    get_filename_component(SVENCOOP_MSVC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(SVENCOOP_MSVC_TOOLSET_DIR "${SVENCOOP_MSVC_BIN_DIR}/../../.." ABSOLUTE)
    set(SVENCOOP_FLOAT_ROUNDING_OBJ "${SVENCOOP_MSVC_TOOLSET_DIR}/lib/x86/legacy_stdio_float_rounding.obj")
    if(EXISTS "${SVENCOOP_FLOAT_ROUNDING_OBJ}")
        target_link_options(svencoop_server PRIVATE "${SVENCOOP_FLOAT_ROUNDING_OBJ}")
    endif()
    set_target_properties(svencoop_server PROPERTIES OUTPUT_NAME "server")
else()
    target_compile_definitions(svencoop_server PRIVATE DISABLE_VEC_ORIGIN)
    target_compile_options(svencoop_server PRIVATE -fexceptions)
    # CPlayerDatabase uses the system libcurl on non-Windows platforms.
    find_package(CURL REQUIRED)
    target_link_libraries(svencoop_server PRIVATE m dl pthread CURL::libcurl)
    if(SVENCOOP_TARGET_64BIT)
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
    if(APPLE)
        set_target_properties(svencoop_server PROPERTIES SUFFIX ".dylib")
    endif()
endif()

set_target_properties(svencoop_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SVENCOOP_RUNTIME_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${SVENCOOP_LIBRARY_OUTPUT_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${SVENCOOP_ARCHIVE_OUTPUT_DIR}"
)
