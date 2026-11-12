//========= Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
	   
#ifndef PLATFORM_H
#define PLATFORM_H

#include "basetypes.h"

#ifdef _WIN32
#pragma once
#endif

#ifndef V_min
#define V_min(a,b)  (((a) < (b)) ? (a) : (b))
#define V_max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#include "archtypes.h"     // DAL

// Misc C-runtime library headers
#ifdef __cplusplus
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#endif


#ifdef _WIN32
// Avoid the ISO conformant warning
#define stricmp _stricmp
#define strnicmp _strnicmp
#define itoa _itoa
#define strupr _strupr

#ifdef VALVE_DLL
#define DLL_EXPORT extern "C" __declspec( dllexport )
#define DLL_IMPORT extern "C" __declspec( dllimport )
#endif

#define DLLEXPORT __declspec( dllexport )
#else
#ifdef VALVE_DLL
#define DLL_EXPORT extern "C" __attribute__( ( visibility( "default" ) ) )
#define DLL_IMPORT extern "C"
#endif

#define DLLEXPORT __attribute__( ( visibility( "default" ) ) )
#endif

#define DECLARE_CLASS_SERVER( className, baseClassName ) \
	typedef baseClassName BaseClass; \
	typedef className ThisClass;

// feature enables
#define NEW_SOFTWARE_LIGHTING

#ifdef POSIX
// need this for _alloca
#include <alloca.h>
#endif

#include <malloc.h>
#ifdef __cplusplus
#include <new>
#endif

// need this for memset
#include <string.h>

#ifdef _RETAIL
#define IsRetail() true 
#else
#define IsRetail() false
#endif

#ifdef _DEBUG
#define IsRelease() false
#define IsDebug() true
#else
#define IsRelease() true
#define IsDebug() false
#endif

#define IsXbox()	false

#ifdef _WIN32
#define IsLinux() false
#define IsOSX() false
#define IsPosix() false
#define PLATFORM_WINDOWS 1 // Windows PC or Xbox 360

#define IsWindows() true
#define IsPC() true
#define IsConsole() false
#define IsX360() false
#define IsPS3() false
#define IS_WINDOWS_PC
#define PLATFORM_WINDOWS_PC 1 // Windows PC
#ifdef _WIN64
#define IsPlatformWindowsPC64() true
#define IsPlatformWindowsPC32() false
#define PLATFORM_WINDOWS_PC64 1
#else
#define IsPlatformWindowsPC64() false
#define IsPlatformWindowsPC32() true
#define PLATFORM_WINDOWS_PC32 1
#endif

#define IsPlatformOpenGL() true
#elif defined( POSIX )

#define IsLinux() true
#define IsOSX() false
#define IsPosix() true
#define IsWindows() false
#define IsPC() true
#define IsConsole() false
#define IsX360() false
#define IsPS3() false
#define IsPlatformWindowsPC64() false
#define IsPlatformWindowsPC32() false
#define PLATFORM_LINUX 1	

#else
#error
#endif

typedef unsigned char uint8;
typedef signed char int8;

#ifdef __x86_64__
#define X64BITS
#endif

#if defined( _WIN32 )

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;


#ifdef X64BITS
typedef __int64 intp;				// intp is an integer that can accomodate a pointer
typedef unsigned __int64 uintp;		// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
#else
typedef __int32 intp;
typedef unsigned __int32 uintp;
#endif

#else // _WIN32

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#ifdef X64BITS
typedef long long intp;
typedef unsigned long long uintp;
#else
typedef int intp;
typedef unsigned int uintp;
#endif

#endif // else _WIN32

#undef OVERRIDE
#define OVERRIDE override
#if defined(__clang__)
// warning: 'override' keyword is a C++11 extension [-Wc++11-extensions]
// Disabling this warning is less intrusive than enabling C++11 extensions
#pragma GCC diagnostic ignored "-Wc++11-extensions"
#endif


typedef float float32;
typedef double float64;

// for when we don't care about how many bits we use
typedef unsigned int uint;

#define XBOX_DVD_SECTORSIZE	2048
#define XBOX_HDD_SECTORSIZE	512

// Custom windows messages for Xbox input
#define WM_XREMOTECOMMAND			WM_USER + 100
#define WM_XCONTROLLER_KEY			WM_USER + 101
#define WM_XCONTROLLER_UNPLUGGED	WM_USER + 102

// This can be used to ensure the size of pointers to members when declaring
// a pointer type for a class that has only been forward declared
#ifdef _MSC_VER
#define SINGLE_INHERITANCE __single_inheritance 
#define MULTIPLE_INHERITANCE __multiple_inheritance 
#else
#define SINGLE_INHERITANCE  
#define MULTIPLE_INHERITANCE 
#endif

#ifdef _MSC_VER
#define NO_VTABLE __declspec( novtable )
#else
#define NO_VTABLE
#endif

// This can be used to declare an abstract (interface only) class. 
// Classes marked abstract should not be instantiated.  If they are, and access violation will occur.
//
// Example of use:
// 
// abstract_class CFoo
// {
//      ...
// }
//
// MSDN __declspec(novtable) documentation: http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vclang/html/_langref_novtable.asp
//
// Note: NJS: This is not enabled for regular PC, due to not knowing the implications of exporting a class with no no vtable.
//       It's probable that this shouldn't be an issue, but an experiment should be done to verify this.
//
#ifndef _XBOX
#define abstract_class class
#else
#define abstract_class class NO_VTABLE
#endif

#define VALVE_RAND_MAX 0x7fff

/*
FIXME: Enable this when we no longer fear change =)

// need these for the limits
#include <limits.h>
#include <float.h>

// Maximum and minimum representable values
#define  INT8_MAX    SCHAR_MAX
#define  INT16_MAX   SHRT_MAX
#define  INT32_MAX   LONG_MAX
#define  INT64_MAX   (((int64)~0) >> 1)

#define  INT8_MIN    SCHAR_MIN
#define  INT16_MIN   SHRT_MIN
#define  INT32_MIN   LONG_MIN
#define  INT64_MIN   (((int64)1) << 63)

#define  UINT8_MAX   ((uint8)~0)
#define  UINT16_MAX  ((uint16)~0)
#define  UINT32_MAX  ((uint32)~0)
#define  UINT64_MAX  ((uint64)~0)

#define  UINT8_MIN   0
#define  UINT16_MIN  0
#define  UINT32_MIN  0
#define  UINT64_MIN  0

#ifndef  UINT_MIN
#define  UINT_MIN    UINT32_MIN
#endif

#define  FLOAT32_MAX FLT_MAX
#define  FLOAT64_MAX DBL_MAX

#define  FLOAT32_MIN FLT_MIN
#define  FLOAT64_MIN DBL_MIN
*/

// portability / compiler settings
#if defined(_WIN32) && !defined(WINDED)

#if defined(_M_IX86)
#define __i386__	1
#endif

// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH  260
#endif

#elif POSIX
#define _vsnprintf vsnprintf

typedef unsigned int DWORD;
//typedef unsigned short WORD;
typedef void* HINSTANCE;

#define _MAX_PATH PATH_MAX
#define __cdecl
#define __declspec

// Defines MAX_PATH
#ifndef MAX_PATH
#define MAX_PATH  260
#endif
#endif // defined(_WIN32) && !defined(WINDED)

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) //  need macro for constant expression

// Used to step into the debugger
#ifdef _WIN32
#define DebuggerBreak()  __asm { int 3 }
#else
#define DebuggerBreak()  {}
#endif

// C functions for external declarations that call the appropriate C++ methods
#ifndef EXPORT
#ifdef _WIN32
#define EXPORT	_declspec( dllexport )
#else 
#define EXPORT	__attribute__((visibility("default")))
#endif
#endif

#if defined __i386__ && !defined __linux__
#define id386	1
#else
#define id386	0
#endif  // __i386__

// decls for aligning data
#ifdef _WIN32
#define DECL_ALIGN(x) __declspec(align(x))

#elif GNUC
#define DECL_ALIGN(x) __attribute__((aligned(x)))
#else
#define DECL_ALIGN(x) /* */
#endif

#ifdef _MSC_VER
// MSVC has the align at the start of the struct
#define ALIGN4 DECL_ALIGN(4)
#define ALIGN8 DECL_ALIGN(8)
#define ALIGN16 DECL_ALIGN(16)
#define ALIGN32 DECL_ALIGN(32)
#define ALIGN128 DECL_ALIGN(128)

#define ALIGN4_POST
#define ALIGN8_POST
#define ALIGN16_POST
#define ALIGN32_POST
#define ALIGN128_POST
#elif defined( GNUC )
// gnuc has the align decoration at the end
#define ALIGN4
#define ALIGN8 
#define ALIGN16
#define ALIGN32
#define ALIGN128

#define ALIGN4_POST DECL_ALIGN(4)
#define ALIGN8_POST DECL_ALIGN(8)
#define ALIGN16_POST DECL_ALIGN(16)
#define ALIGN32_POST DECL_ALIGN(32)
#define ALIGN128_POST DECL_ALIGN(128)
#else
#error
#endif

//-----------------------------------------------------------------------------
// Convert int<-->pointer, avoiding 32/64-bit compiler warnings:
//-----------------------------------------------------------------------------
#define INT_TO_POINTER( i ) (void *)( ( i ) + (char *)NULL )
#define POINTER_TO_INT( p ) ( (int)(uintp)( p ) )


//-----------------------------------------------------------------------------
// Stack-based allocation related helpers
//-----------------------------------------------------------------------------
#if defined( GNUC )
#define stackalloc( _size )		alloca( ALIGN_VALUE( _size, 16 ) )
#ifdef _LINUX
#define mallocsize( _p )	( malloc_usable_size( _p ) )
#elif defined(OSX)
#define mallocsize( _p )	( malloc_size( _p ) )
#else
#error
#endif
#elif defined ( _WIN32 )
#define stackalloc( _size )		_alloca( ALIGN_VALUE( _size, 16 ) )
#define mallocsize( _p )		( _msize( _p ) )
#endif

#define  stackfree( _p )			0

// Linux had a few areas where it didn't construct objects in the same order that Windows does.
// So when CVProfile::CVProfile() would access g_pMemAlloc, it would crash because the allocator wasn't initalized yet.
#ifdef POSIX
#define CONSTRUCT_EARLY __attribute__((init_priority(101)))
#else
#define CONSTRUCT_EARLY
#endif

#if defined(_MSC_VER)
#define SELECTANY __declspec(selectany)
#define RESTRICT __restrict
#define RESTRICT_FUNC __declspec(restrict)
#define FMTFUNCTION( a, b )
#elif defined(GNUC)
#define SELECTANY __attribute__((weak))
#if defined(LINUX) && !defined(DEDICATED)
#define RESTRICT
#else
#define RESTRICT __restrict
#endif
#define RESTRICT_FUNC
// squirrel.h does a #define printf DevMsg which leads to warnings when we try
// to use printf as the prototype format function. Using __printf__ instead.
#define FMTFUNCTION( fmtargnumber, firstvarargnumber ) __attribute__ (( format( __printf__, fmtargnumber, firstvarargnumber )))
#else
#define SELECTANY static
#define RESTRICT
#define RESTRICT_FUNC
#define FMTFUNCTION( a, b )
#endif

#ifdef __cplusplus
#define C_EXTERN extern "C"
#else
#define C_EXTERN extern
#endif

#ifdef __cplusplus
#define DECLTYPE(func) (decltype(func))
#else
#define DECLTYPE(func) (void*)
#endif

#if defined( _WIN32 )

// Used for dll exporting and importing
#ifndef DLL_EXPORT
#define DLL_EXPORT				C_EXTERN __declspec( dllexport )
#endif
#ifndef DLL_IMPORT
#define DLL_IMPORT				C_EXTERN __declspec( dllimport )
#endif

// Can't use extern "C" when DLL exporting a class
#define DLL_CLASS_EXPORT		__declspec( dllexport )
#define DLL_CLASS_IMPORT		__declspec( dllimport )

// Can't use extern "C" when DLL exporting a global
#define DLL_GLOBAL_EXPORT		extern __declspec( dllexport )
#define DLL_GLOBAL_IMPORT		extern __declspec( dllimport )

#define DLL_LOCAL

#elif defined GNUC
// Used for dll exporting and importing
#define  DLL_EXPORT   C_EXTERN __attribute__ ((visibility("default")))
#define  DLL_IMPORT   C_EXTERN

// Can't use extern "C" when DLL exporting a class
#define  DLL_CLASS_EXPORT __attribute__ ((visibility("default")))
#define  DLL_CLASS_IMPORT

// Can't use extern "C" when DLL exporting a global
#define  DLL_GLOBAL_EXPORT   extern __attribute ((visibility("default")))
#define  DLL_GLOBAL_IMPORT   extern

#define  DLL_LOCAL __attribute__ ((visibility("hidden"))) 
#else
#error "Unsupported Platform."
#endif

// Used for standard calling conventions
#ifdef _WIN32
#define  STDCALL				__stdcall
#define  FASTCALL				__fastcall
#define  FORCEINLINE			__forceinline	
// GCC 3.4.1 has a bug in supporting forced inline of templated functions
// this macro lets us not force inlining in that case
#define  FORCEINLINE_TEMPLATE		__forceinline	
#else
#define  STDCALL
#define  FASTCALL 
#define  FORCEINLINE			__attribute__ ((always_inline)) inline
// GCC 3.4.1 has a bug in supporting forced inline of templated functions
// this macro lets us not force inlining in that case
#define  FORCEINLINE_TEMPLATE
#define  __stdcall			__attribute__ ((__stdcall__)) 
#endif

// Force a function call site -not- to inlined. (useful for profiling)
#define DONT_INLINE(a) (((int)(a)+1)?(a):(a))

// Pass hints to the compiler to prevent it from generating unnessecary / stupid code
// in certain situations.  Several compilers other than MSVC also have an equivilent 
// construct.
//
// Essentially the 'Hint' is that the condition specified is assumed to be true at 
// that point in the compilation.  If '0' is passed, then the compiler assumes that
// any subsequent code in the same 'basic block' is unreachable, and thus usually 
// removed.
#ifdef _MSC_VER
#define HINT(THE_HINT)	__assume((THE_HINT))
#else
#define HINT(THE_HINT)	0
#endif

// Marks the codepath from here until the next branch entry point as unreachable,
// and asserts if any attempt is made to execute it.
#define UNREACHABLE() { Assert(0); HINT(0); }

// In cases where no default is present or appropriate, this causes MSVC to generate 
// as little code as possible, and throw an assertion in debug.
#define NO_DEFAULT default: UNREACHABLE();

#ifdef _WIN32
// Remove warnings from warning level 4.
#pragma warning(disable : 4514) // warning C4514: 'acosl' : unreferenced inline function has been removed
#pragma warning(disable : 4100) // warning C4100: 'hwnd' : unreferenced formal parameter
#pragma warning(disable : 4127) // warning C4127: conditional expression is constant
#pragma warning(disable : 4512) // warning C4512: 'InFileRIFF' : assignment operator could not be generated
#pragma warning(disable : 4611) // warning C4611: interaction between '_setjmp' and C++ object destruction is non-portable
#pragma warning(disable : 4710) // warning C4710: function 'x' not inlined
#pragma warning(disable : 4702) // warning C4702: unreachable code
#pragma warning(disable : 4505) // unreferenced local function has been removed
#pragma warning(disable : 4239) // nonstandard extension used : 'argument' ( conversion from class Vector to class Vector& )
#pragma warning(disable : 4097) // typedef-name 'BaseClass' used as synonym for class-name 'CFlexCycler::CBaseFlex'
#pragma warning(disable : 4324) // Padding was added at the end of a structure
#pragma warning(disable : 4244) // type conversion warning.
#pragma warning(disable : 4305)	// truncation from 'const double ' to 'float '
#pragma warning(disable : 4786)	// Disable warnings about long symbol names
#pragma warning(disable : 4250) // 'X' : inherits 'Y::Z' via dominance

#if _MSC_VER >= 1300
#pragma warning(disable : 4511)	// Disable warnings about private copy constructors
#pragma warning(disable : 4121)	// warning C4121: 'symbol' : alignment of a member was sensitive to packing
#pragma warning(disable : 4530)	// warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc (disabled due to std headers having exception syntax)
#endif

#if _MSC_VER >= 1400
#pragma warning(disable : 4996)	// functions declared deprecated
#endif


#endif

// When we port to 64 bit, we'll have to resolve the int, ptr vs size_t 32/64 bit problems...
#if !defined( _WIN64 ) && defined( WIN32 )
#pragma warning( disable : 4267 )	// conversion from 'size_t' to 'int', possible loss of data
#pragma warning( disable : 4311 )	// pointer truncation from 'char *' to 'int'
#pragma warning( disable : 4312 )	// conversion from 'unsigned int' to 'memhandle_t' of greater size
#endif

#ifdef POSIX
#ifndef _stricmp
#define _stricmp stricmp
#endif
#define strcmpi stricmp
#define stricmp strcasecmp
#define _vsnprintf vsnprintf
#ifndef _alloca
#define _alloca alloca
#endif
#ifdef _snprintf
#undef _snprintf
#endif
#define _snprintf snprintf
#define GetProcAddress dlsym
#define _chdir chdir
#ifndef _strnicmp
#define _strnicmp strnicmp
#endif
#define strnicmp strncasecmp
#define _getcwd getcwd
#define _snwprintf swprintf
#define swprintf_s swprintf
#define wcsicmp _wcsicmp
#define _wcsicmp wcscmp
#define _finite finite
#define _tempnam tempnam
#define _unlink unlink
#define _access access
#define _mkdir(dir) mkdir( dir, S_IRWXU | S_IRWXG | S_IRWXO )
#define _wtoi(arg) wcstol(arg, NULL, 10)
#define _wtoi64(arg) wcstoll(arg, NULL, 10)

//#ifndef HMODULE
//typedef uint32 HMODULE;
//#endif
#ifndef HANDLE
typedef void* HANDLE;
#endif

#endif

//-----------------------------------------------------------------------------
// FP exception handling
//-----------------------------------------------------------------------------
//#define CHECK_FLOAT_EXCEPTIONS 1

#ifdef _MSC_VER

inline void SetupFPUControlWordForceExceptions()
{
	// use local to get and store control word
	uint16 tmpCtrlW;              
	__asm 
	{
		fnclex                     /* clear all current exceptions */
		fnstcw word ptr [tmpCtrlW] /* get current control word */
		and [tmpCtrlW], 0FCC0h     /* Keep infinity control + rounding control */
		or [tmpCtrlW],   0230h     /* set to 53-bit, mask only inexact, underflow */
		fldcw word ptr [tmpCtrlW]  /* put new control word in FPU */
	}
}

#ifdef CHECK_FLOAT_EXCEPTIONS
	 
inline void SetupFPUControlWord()
{
	SetupFPUControlWordForceExceptions();
}

#else

inline void SetupFPUControlWord()
{
	// use local to get and store control word
	uint16 tmpCtrlW;
	__asm 
	{
		fnstcw word ptr [tmpCtrlW] /* get current control word */
		and [tmpCtrlW], 0FCC0h     /* Keep infinity control + rounding control */
		or [tmpCtrlW],   023Fh     /* set to 53-bit, mask only inexact, underflow */
		fldcw word ptr [tmpCtrlW]  /* put new control word in FPU */
	}
}

#endif

#else

inline void SetupFPUControlWord()
{
}

#endif // _MSC_VER

#ifdef __cplusplus

//-----------------------------------------------------------------------------
// Purpose: Standard functions for handling endian-ness
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// DLL export for platform utilities
//-----------------------------------------------------------------------------
#ifndef STATIC_TIER0
#ifdef TIER0_DLL_EXPORT
#define PLATFORM_INTERFACE	DLL_EXPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_EXPORT
#define PLATFORM_CLASS		DLL_CLASS_EXPORT
#else
#define PLATFORM_INTERFACE	DLL_IMPORT
#define PLATFORM_OVERLOAD	DLL_GLOBAL_IMPORT
#define PLATFORM_CLASS		DLL_CLASS_IMPORT
#endif
#else	// BUILD_AS_DLL
#define PLATFORM_INTERFACE	extern
#define PLATFORM_OVERLOAD
#define PLATFORM_CLASS
#endif	// BUILD_AS_DLL

#if _X360
#define Plat_FastMemset XMemSet
#define Plat_FastMemcpy XMemCpy
#else
#define Plat_FastMemset memset
#define Plat_FastMemcpy memcpy
#endif

//-----------------------------------------------------------------------------

#if defined(_INC_WINDOWS) && defined(_WIN32)
template <typename FUNCPTR_TYPE>
class CDynamicFunction
{
public:
	CDynamicFunction(const char* pszModule, const char* pszName, FUNCPTR_TYPE pfnFallback = NULL)
	{
		m_pfn = pfnFallback;

		HMODULE hModule = ::LoadLibrary(pszModule);
		if (hModule)
			m_pfn = (FUNCPTR_TYPE)::GetProcAddress(hModule, pszName);
	}

	operator bool() { return m_pfn != NULL; }
	bool operator !() { return !m_pfn; }
	operator FUNCPTR_TYPE() { return m_pfn; }

private:
	FUNCPTR_TYPE m_pfn;
};
#endif

//-----------------------------------------------------------------------------

#endif // #ifdef __cplusplus

#endif /* PLATFORM_H */