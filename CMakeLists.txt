# Sven Co-op SDK - cross-platform CMake build
#
# Builds the two GoldSrc modules shipped by this repository:
#   * server  (server.dll / hl_i386.so / hl_i386.dylib)
#   * client  (client.dll / client.so / client.dylib)
#
# The code is 32-bit only: the GoldSrc engine and the prebuilt dependencies
# (vgui, SDL2, FMOD Ex, Discord RPC, curl) are x86. See README.md for the
# supported configure commands.

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

option(SVENCOOP_BUILD_SERVER "Build the server module" ON)
option(SVENCOOP_BUILD_CLIENT "Build the client module" ON)
option(SVENCOOP_WARNINGS    "Enable the compiler warning level used by the old projects" ON)
option(SVENCOOP_ALLOW_64BIT "Allow a 64-bit Linux build (requires 64-bit dependencies)" OFF)

# ---------------------------------------------------------------------------
# Architecture checks
# ---------------------------------------------------------------------------
if(WIN32 AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR
        "This project is 32-bit only. Configure the Visual Studio generator with "
        "'-A Win32' (e.g. cmake -S . -B build -G \"Visual Studio 17 2022\" -A Win32).")
endif()

if(UNIX AND NOT APPLE AND CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT SVENCOOP_ALLOW_64BIT)
    message(FATAL_ERROR
        "GoldSrc and the bundled dependencies are 32-bit. Configure with the supplied "
        "toolchain file: cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-linux-x86.cmake. "
        "Set SVENCOOP_ALLOW_64BIT=ON only if you have 64-bit replacements for vgui/SDL2/FMODEx.")
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
    # POSIX/GCC definitions shared by the server and client modules.
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

# Crypto++ selects its SIMD implementations at compile time on GCC/Clang.
if(NOT MSVC)
    set_source_files_properties(external/cryptopp/gcm_simd.cpp      PROPERTIES COMPILE_OPTIONS "-mssse3;-mpclmul")
    set_source_files_properties(external/cryptopp/rijndael_simd.cpp PROPERTIES COMPILE_OPTIONS "-msse4.1;-maes")
    set_source_files_properties(external/cryptopp/sha_simd.cpp      PROPERTIES COMPILE_OPTIONS "-msse4.2;-msha")
    set_source_files_properties(external/cryptopp/sse_simd.cpp      PROPERTIES COMPILE_OPTIONS "-msse2")
endif()

# ---------------------------------------------------------------------------
# Include directories / definitions shared by the game modules
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
# The server only ships libcurl_sven.def; the old project generated the import
# library with lib.exe before linking.
# ---------------------------------------------------------------------------
if(WIN32 AND MSVC AND SVENCOOP_BUILD_SERVER)
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
# Output directories
# ---------------------------------------------------------------------------
set(SVENCOOP_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(SVENCOOP_LIBRARY_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(SVENCOOP_ARCHIVE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/lib")

# ---------------------------------------------------------------------------
# Server module
# ---------------------------------------------------------------------------
if(SVENCOOP_BUILD_SERVER)
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
    )
    target_compile_definitions(svencoop_server PRIVATE
        ${SVENCOOP_COMMON_DEFINITIONS}
        VALVE_DLL
        CLIENT_WEAPONS
        AS_PROCESS_METADATA=0
        AS_USE_STLNAMES=1
        AS_USE_ACCESSORS=0
    )

    target_link_libraries(svencoop_server PRIVATE svencoop_server_runtime)

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
        # Keep the file names produced by the former Linux Makefiles.
        set_target_properties(svencoop_server PROPERTIES
            PREFIX ""
            OUTPUT_NAME "hl_i386")
        if(APPLE)
            set_target_properties(svencoop_server PROPERTIES SUFFIX ".dylib")
        endif()
    endif()

    set_target_properties(svencoop_server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SVENCOOP_RUNTIME_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${SVENCOOP_LIBRARY_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${SVENCOOP_ARCHIVE_OUTPUT_DIR}"
    )
endif()

# ---------------------------------------------------------------------------
# Client module
# ---------------------------------------------------------------------------
if(SVENCOOP_BUILD_CLIENT)
    add_library(svencoop_client MODULE
        ${SVENCOOP_CLIENT_SOURCES}
    )

    target_include_directories(svencoop_client PRIVATE
        ${SVENCOOP_COMMON_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/cl_dll
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/vgui/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
        ${CMAKE_CURRENT_SOURCE_DIR}/external/fmodex/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/discord-rpc/include
    )
    target_compile_definitions(svencoop_client PRIVATE
        ${SVENCOOP_COMMON_DEFINITIONS}
        CLIENT_DLL
        CLIENT_WEAPONS
        HL_DLL
    )

    if(WIN32)
        target_compile_options(svencoop_client PRIVATE /EHsc)
        target_link_libraries(svencoop_client PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/utils/vgui/lib/win32_vc6/vgui.lib"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/public/SDL2.lib"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/public/fmodex_vc.lib"
            "${CMAKE_CURRENT_SOURCE_DIR}/lib/public/discord-rpc.lib"
            opengl32
            wsock32
            delayimp
        )
        target_link_options(svencoop_client PRIVATE "/DELAYLOAD:fmodex.dll")
        set_target_properties(svencoop_client PROPERTIES OUTPUT_NAME "client")
    else()
        target_link_libraries(svencoop_client PRIVATE m dl pthread)
        target_compile_options(svencoop_client PRIVATE -fexceptions)
        target_compile_definitions(svencoop_client PRIVATE _snwprintf=swprintf)
        if(APPLE)
            target_compile_definitions(svencoop_client PRIVATE DISABLE_VEC_ORIGIN)
            target_link_libraries(svencoop_client PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/linux/release/vgui.dylib"
                "${CMAKE_CURRENT_SOURCE_DIR}/linux/libSDL2-2.0.0.dylib"
            )
            find_library(SVENCOOP_DISCORD_RPC_LIBRARY NAMES discord-rpc)
            find_library(SVENCOOP_FMODEX_LIBRARY NAMES fmodex)
            if(SVENCOOP_DISCORD_RPC_LIBRARY)
                target_link_libraries(svencoop_client PRIVATE "${SVENCOOP_DISCORD_RPC_LIBRARY}")
            endif()
            if(SVENCOOP_FMODEX_LIBRARY)
                target_link_libraries(svencoop_client PRIVATE "${SVENCOOP_FMODEX_LIBRARY}")
            endif()
            target_link_libraries(svencoop_client PRIVATE "-framework Carbon" "-framework OpenGL")
        else()
            target_compile_definitions(svencoop_client PRIVATE DISABLE_VEC_ORIGIN)
            # Bundled 32-bit libraries: vgui and SDL2 ship in linux/.
            target_link_directories(svencoop_client PRIVATE
                "${CMAKE_CURRENT_SOURCE_DIR}/linux")

            set(SVENCOOP_DISCORD_RPC_LIBRARY "" CACHE FILEPATH
                "Path to the 32-bit Discord RPC library")
            set(SVENCOOP_FMODEX_LIBRARY "" CACHE FILEPATH
                "Path to the 32-bit FMOD Ex library")
            set(SVENCOOP_OPENGL_LIBRARY "" CACHE FILEPATH
                "Path to the 32-bit OpenGL library")

            if(NOT SVENCOOP_DISCORD_RPC_LIBRARY)
                find_library(SVENCOOP_DISCORD_RPC_LIBRARY NAMES discord-rpc discord_rpc)
            endif()
            if(NOT SVENCOOP_FMODEX_LIBRARY)
                find_library(SVENCOOP_FMODEX_LIBRARY NAMES fmodex)
            endif()
            if(NOT SVENCOOP_OPENGL_LIBRARY)
                find_library(SVENCOOP_OPENGL_LIBRARY NAMES GL)
            endif()

            if(NOT SVENCOOP_DISCORD_RPC_LIBRARY)
                message(FATAL_ERROR
                    "A 32-bit Discord RPC library was not found. Pass "
                    "-DSVENCOOP_DISCORD_RPC_LIBRARY=/path/to/libdiscord-rpc.so")
            endif()
            if(NOT SVENCOOP_FMODEX_LIBRARY)
                message(FATAL_ERROR
                    "A 32-bit FMOD Ex library was not found. Pass "
                    "-DSVENCOOP_FMODEX_LIBRARY=/path/to/libfmodex.so")
            endif()
            if(NOT SVENCOOP_OPENGL_LIBRARY)
                message(FATAL_ERROR
                    "A 32-bit OpenGL library was not found. Install the i386 "
                    "libgl1-mesa-dev package or pass -DSVENCOOP_OPENGL_LIBRARY=/path/to/libGL.so")
            endif()

            target_link_libraries(svencoop_client PRIVATE
                # vgui.so has no SONAME; -l: links the exact file name.
                "-l:vgui.so"
                "${CMAKE_CURRENT_SOURCE_DIR}/linux/libSDL2-2.0.so.0"
                "${SVENCOOP_OPENGL_LIBRARY}"
                "${SVENCOOP_DISCORD_RPC_LIBRARY}"
                "${SVENCOOP_FMODEX_LIBRARY}"
            )
        endif()
        set_target_properties(svencoop_client PROPERTIES
            PREFIX ""
            OUTPUT_NAME "client")
        if(APPLE)
            set_target_properties(svencoop_client PROPERTIES SUFFIX ".dylib")
        endif()
    endif()

    set_target_properties(svencoop_client PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${SVENCOOP_RUNTIME_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${SVENCOOP_LIBRARY_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${SVENCOOP_ARCHIVE_OUTPUT_DIR}"
    )
endif()
