/****************************************************************************
**
** Global type declarations and definitions
**
** Created : 920529
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQGLOBAL_H
#define TQGLOBAL_H

#define TQT_VERSION_STR   "3.5.0"
/*
   TQT_VERSION is (major << 16) + (minor << 8) + patch.
 */
#define TQT_VERSION 0x030500

/*
   The operating system, must be one of: (Q_OS_x)

     MACX	- Mac OS X
     MAC9	- Mac OS 9
     DARWIN     - Darwin OS (Without Mac OS X)
     MSDOS	- MS-DOS and Windows
     OS2	- OS/2
     OS2EMX	- XFree86 on OS/2 (not PM)
     WIN32	- Win32 (Windows 95/98/ME and Windows NT/2000/XP)
     CYGWIN	- Cygwin
     SOLARIS	- Sun Solaris
     ULTRIX	- DEC Ultrix
     LINUX	- Linux
     FREEBSD	- FreeBSD
     NETBSD	- NetBSD
     OPENBSD	- OpenBSD
     BSDI	- BSD/OS
     UNIXWARE	- UnixWare 7, Open UNIX 8
     AIX	- AIX
     HURD	- GNU Hurd
     DYNIX	- DYNIX/ptx
     TQNX	- TQNX
     TQNX6	- TQNX RTP 6.1
     LYNX	- LynxOS
     BSD4	- Any BSD 4.4 system
     UNIX	- Any UNIX BSD/SYSV system
*/

#if defined(__DARWIN_X11__)
#  define Q_OS_DARWIN
#elif defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__))
#  define Q_OS_MACX
#elif defined(__MACOSX__)
#  define Q_OS_MACX
#elif defined(macintosh)
#  define Q_OS_MAC9
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define Q_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define Q_OS_OS2EMX
#  else
#    define Q_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define Q_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define Q_OS_SOLARIS
#elif defined(__ultrix) || defined(ultrix)
#  define Q_OS_ULTRIX
#elif defined(__GNU__)
#  define Q_OS_HURD
#elif defined(__linux__) || defined(__linux) || defined(__GNU__) || defined(__GLIBC__)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(_AIX)
#  define Q_OS_AIX
#elif defined(__Lynx__)
#  define Q_OS_LYNX
#elif defined(__QNXNTO__)
#  define Q_OS_QNX6
#elif defined(__QNX__)
#  define Q_OS_QNX
#elif defined(_SEQUENT_)
#  define Q_OS_DYNIX
#elif defined(__USLC__)                  /* all UDK or OUDK */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define Q_OS_UNIXWARE
#  define Q_OS_UNIXWARE7
#elif defined(__MAKEDEPEND__)
#else
#  error "TQt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#  define Q_OS_WIN
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MACX)
#  define Q_OS_MAC
#endif

#if defined(Q_OS_MAC9) || defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif

#if defined(Q_OS_MACX)
#  ifdef MAC_OS_X_VERSION_MIN_REQUIRED
#    undef MAC_OS_X_VERSION_MIN_REQUIRED
#  endif
#  define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_2
#  include <AvailabilityMacros.h>
#  if !defined(MAC_OS_X_VERSION_10_3)
#     define MAC_OS_X_VERSION_10_3 MAC_OS_X_VERSION_10_2 + 1
#  endif
#  if !defined(MAC_OS_X_VERSION_10_4)
#       define MAC_OS_X_VERSION_10_4 MAC_OS_X_VERSION_10_3 + 1
#  endif
#  if (MAC_OS_X_VERSION_MAX_ALLOWED > MAC_OS_X_VERSION_10_4)
#    error "This version of Mac OS X is unsupported"
#  endif
#endif


/*
   The compiler, must be one of: (Q_CC_x)

     SYM	- Symantec C++ for both PC and Macintosh
     MPW	- MPW C++
     MSVC	- Microsoft Visual C/C++, Intel C++ for Windows
     GNU	- GNU C++
     COMEAU	- Comeau C++
     EDG	- Edison Design Group C++
     OC		- CenterLine C++
     SUN	- Sun WorkShop, Forte Developer, or Sun ONE Studio C++
     MIPS	- MIPSpro C++
     DEC	- DEC C++
     USLC	- UDK, and UnixWare 2.X C++
     KAI	- KAI C++
     INTEL	- Intel C++ for Linux, Intel C++ for Windows
     HIGHC	- MetaWare High C/C++
     PGI	- Portland Group C++
     GHS	- Green Hills Optimizing C++ Compilers

   Should be sorted most to least authoritative.
*/

/* Symantec C++ is now Digital Mars */
#if defined(__DMC__) || defined(__SC__)
#  define Q_CC_SYM
#  define Q_NO_USING_KEYWORD
#  if !defined(_CPPUNWIND)
#    define Q_NO_EXCEPTIONS
#  endif

#elif defined(applec)
#  define Q_CC_MPW
#  define Q_NO_USING_KEYWORD

#elif defined(_MSC_VER)
#  define Q_CC_MSVC
/* Visual C++.Net issues for _MSC_VER >= 1300 */
#  if _MSC_VER >= 1300
#    define Q_CC_MSVC_NET
#    if _MSC_VER < 1310 || (defined(Q_OS_WIN64) && defined(_M_IA64))
#      define TQ_TYPENAME
#    endif
#  endif
/* Intel C++ disguising as Visual C++: the `using' keyword avoids warnings */
#  if defined(__INTEL_COMPILER)
#    define Q_CC_INTEL
#    if !defined(__EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif
#  else
#    define Q_NO_USING_KEYWORD /* ### check "using" status */
#  endif

#elif defined(__GNUC__)
#  define Q_CC_GNU
#  define Q_C_CALLBACKS
/* GCC 2.95 knows "using" but does not support it correctly */
#  if __GNUC__ == 2 && __GNUC_MINOR__ <= 95
#    define Q_NO_USING_KEYWORD
#  endif

/* ARM gcc pads structs to 32 bits, even when they contain a single
   char, or short.  We tell gcc to pack TQChars to 16 bits, to avoid
   TQString bloat. However, gcc 3.4 doesn't allow us to create references to
   members of a packed struct. (Pointers are OK, because then you
   supposedly know what you are doing.) */
#  if (defined(__arm__) || defined(__ARMEL__)) && !defined(__ARM_EABI__) && !defined(QT_MOC_CPP)
#    define TQ_PACKED __attribute__ ((packed))
#    if __GNUC__ == 3 && __GNUC_MINOR__ >= 4 || __GNUC__ > 3
#      define Q_NO_PACKED_REFERENCE
#    endif
#  endif
#  if !defined(__EXCEPTIONS)
#    define Q_NO_EXCEPTIONS
#  endif

/* IBM compiler versions are a bit messy. There are actually two products:
   the C product, and the C++ product. The C++ compiler is always packaged
   with the latest version of the C compiler. Version numbers do not always
   match. This little table (I'm not sure it's accurate) should be helpful:

   C++ product                C product

            C Set 3.1         C Compiler 3.0
                  ...         ...
   C++ Compiler 3.6.6         C Compiler 4.3
                  ...         ...
   Visual Age C++ 4.0         ...
                  ...         ...
   Visual Age C++ 5.0         C Compiler 5.0
                  ...         ...
   Visual Age C++ 6.0         C Compiler 6.0

   Now:
   __xlC__    is the version of the C compiler in hexadecimal notation
              is only an approximation of the C++ compiler version
   __IBMCPP__ is the version of the C++ compiler in decimal notation
              but it is not defined on older compilers like C Set 3.1 */
#elif defined(__xlC__)
#  define Q_CC_XLC
#  if __xlC__ < 0x400
#    define Q_NO_USING_KEYWORD
#    define TQ_TYPENAME
#  endif

/* Compilers with EDG front end are similar. To detect them we test:
   __EDG documented by SGI, observed on MIPSpro 7.3.1.1 and KAI C++ 4.0b
   __EDG__ documented in EDG online docs, observed on Compaq C++ V6.3-002 */
#elif defined(__EDG) || defined(__EDG__)
#  define Q_CC_EDG

/* The Comeau compiler is based on EDG and does define __EDG__ */
#  if defined(__COMO__)
#    define Q_CC_COMEAU
#    define Q_C_CALLBACKS

/* The `using' keyword was introduced to avoid KAI C++ warnings
   but it's now causing KAI C++ errors instead. The standard is
   unclear about the use of this keyword, and in practice every
   compiler is using its own set of rules. Forget it. */
#  elif defined(__KCC)
#    define Q_CC_KAI
#    if !defined(_EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif
#    define Q_NO_USING_KEYWORD

/* Using the `using' keyword avoids Intel C++ for Linux warnings */
#  elif defined(__INTEL_COMPILER)
#    define Q_CC_INTEL
#    if !defined(__EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif

/* The Portland Group compiler is based on EDG and does define __EDG__ */
#  elif defined(__PGI)
#    define Q_CC_PGI
#    if !defined(__EXCEPTIONS)
#      define Q_NO_EXCEPTIONS
#    endif

/* Never tested! */
#  elif defined(__ghs)
#    define Q_CC_GHS

/* The UnixWare 7 UDK compiler is based on EDG and does define __EDG__ */
#  elif defined(__USLC__) && defined(__SCO_VERSION__)
#    define Q_CC_USLC
#    define Q_NO_USING_KEYWORD /* ### check "using" status */

/* Never tested! */
#  elif defined(CENTERLINE_CLPP) || defined(OBJECTCENTER)
#    define Q_CC_OC
#    define Q_NO_USING_KEYWORD

#  endif

/* The older UnixWare 2.X compiler? */
#elif defined(__USLC__)
#  define Q_CC_USLC
#  define TQ_TYPENAME
#  define Q_NO_USING_KEYWORD

/* Never tested! */
#elif defined(__HIGHC__)
#  define Q_CC_HIGHC

#elif defined(__SUNPRO_CC) || defined(__SUNPRO_C)
#  define Q_CC_SUN
#  if __SUNPRO_CC >= 0x500
#    if defined(__SUNPRO_CC_COMPAT) && (__SUNPRO_CC_COMPAT <= 4)
#      define Q_NO_USING_KEYWORD
#    endif
#    define Q_C_CALLBACKS
/* 4.2 compiler or older */
#  else
#    define Q_NO_USING_KEYWORD
#  endif

#else
#  error "TQt has not been tested with this compiler - talk to qt-bugs@trolltech.com"
#endif

#ifndef TQ_PACKED
#  define TQ_PACKED
#endif


/*
   The window system, must be one of: (TQ_WS_x)

     MACX	- Mac OS X
     MAC9	- Mac OS 9
     TQWS	- TQt/Embedded
     WIN32	- Windows
     X11	- X Window System
     PM		- unsupported
     WIN16	- unsupported
*/

#if defined(Q_OS_MAC9)
#  define TQ_WS_MAC9
#elif defined(Q_OS_MSDOS)
#  define TQ_WS_WIN16
#  error "TQt requires Win32 and does not work with Windows 3.x"
#elif defined(_WIN32_X11_)
#  define TQ_WS_X11
#elif defined(Q_OS_WIN32)
#  define TQ_WS_WIN32
#  if defined(Q_OS_WIN64)
#    define TQ_WS_WIN64
#  endif
#elif defined(Q_OS_OS2)
#  define TQ_WS_PM
#  error "TQt does not work with OS/2 Presentation Manager or Workplace Shell"
#elif defined(Q_OS_UNIX)
#  if defined(TQWS)
#    define TQ_WS_QWS
#    define TQT_NO_QWS_IM
#  elif defined(Q_OS_MACX)
#    define TQ_WS_MACX
#  else
#    define TQ_WS_X11
#  endif
#endif
#if defined(Q_OS_MAC) && !defined(TQMAC_PASCAL)
#  define TQMAC_PASCAL
#endif

#if defined(TQ_WS_WIN16) || defined(TQ_WS_WIN32)
#  define TQ_WS_WIN
#endif

#if (defined(TQ_WS_MAC9) || defined(TQ_WS_MACX)) && !defined(TQ_WS_QWS) && !defined(TQ_WS_X11)
#  define TQ_WS_MAC
#endif


/*
   Some classes do not permit copies to be made of an object.
   These classes contains a private copy constructor and operator=
   to disable copying (the compiler gives an error message).
   Undefine TQ_DISABLE_COPY to turn off this checking.
*/

#define TQ_DISABLE_COPY

#if defined(__cplusplus)


//
// Useful type definitions for TQt
//

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned	uint;
typedef unsigned long   ulong;
typedef char	       *pchar;
typedef uchar	       *puchar;
typedef const char     *pcchar;


//
// Utility macros and inline functions
//

#define TQMAX(a, b)	((b) < (a) ? (a) : (b))
#define TQMIN(a, b)	((a) < (b) ? (a) : (b))
#define TQABS(a)	((a) >= 0  ? (a) : -(a))

inline int tqRound( double d )
{
    return d >= 0.0 ? int(d + 0.5) : int( d - ((int)d-1) + 0.5 ) + ((int)d-1);
}


//
// Size-dependent types (architechture-dependent byte order)
//

typedef signed char		TQ_INT8;		// 8 bit signed
typedef unsigned char		TQ_UINT8;	// 8 bit unsigned
typedef short			TQ_INT16;	// 16 bit signed
typedef unsigned short		TQ_UINT16;	// 16 bit unsigned
typedef int			TQ_INT32;	// 32 bit signed
typedef unsigned int		TQ_UINT32;	// 32 bit unsigned
#if defined(Q_OS_WIN64)
typedef __int64			TQ_LONG;		// word up to 64 bit signed
typedef unsigned __int64	TQ_ULONG;	// word up to 64 bit unsigned
#else
typedef long			TQ_LONG;		// word up to 64 bit signed
typedef unsigned long		TQ_ULONG;	// word up to 64 bit unsigned
#endif
#if defined(Q_OS_WIN) && !defined(Q_CC_GNU)
#  define TQ_INT64_C(c) 		c ## i64	// signed 64 bit constant
#  define TQ_UINT64_C(c)		c ## ui64	// unsigned 64 bit constant
typedef __int64			TQ_INT64;	// 64 bit signed
typedef unsigned __int64	TQ_UINT64;	// 64 bit unsigned
#else
#  define TQ_INT64_C(c) 		c ## LL		// signed 64 bit constant
#  define TQ_UINT64_C(c)		c ## ULL	// unsigned 64 bit constant
typedef long long		TQ_INT64;	// 64 bit signed
typedef unsigned long long	TQ_UINT64;	// 64 bit unsigned
#endif
typedef TQ_INT64			TQ_LLONG;	// signed long long
typedef TQ_UINT64		TQ_ULLONG;	// unsigned long long

#if defined(Q_OS_MACX) && !defined(QT_LARGEFILE_SUPPORT)
#  define QT_LARGEFILE_SUPPORT 64
#endif
#if defined(QT_LARGEFILE_SUPPORT)
    typedef TQ_ULLONG TQtOffset;
#else
    typedef TQ_ULONG TQtOffset;
#endif


//
// Data stream functions is provided by many classes (defined in ntqdatastream.h)
//

class TQCString;
class TQDataStream;
class TQString;

//
// Feature subsetting
//
// Note that disabling some features will produce a libtqt that is not
// compatible with other libtqt builds. Such modifications are only
// supported on TQt/Embedded where reducing the library size is important
// and where the application-suite is often a fixed set.
//

#if !defined(QT_MOC)
#if defined(TQCONFIG_LOCAL)
#include "qconfig-local.h"
#elif defined(TQCONFIG_MINIMAL)
#include "qconfig-minimal.h"
#elif defined(TQCONFIG_SMALL)
#include "qconfig-small.h"
#elif defined(TQCONFIG_MEDIUM)
#include "qconfig-medium.h"
#elif defined(TQCONFIG_LARGE)
#include "qconfig-large.h"
#else // everything...
#include "ntqconfig.h"
#endif
#endif


#ifndef TQT_BUILD_KEY
#define TQT_BUILD_KEY "unspecified"
#endif

// prune to local config
#include "ntqmodules.h"
#ifndef TQT_MODULE_DIALOGS
# define TQT_NO_DIALOG
#endif
#ifndef TQT_MODULE_INPUTMETHOD
# define TQT_NO_IM
#endif
#ifndef TQT_MODULE_WORKSPACE
# define TQT_NO_WORKSPACE
#endif
#ifndef TQT_MODULE_NETWORK
#define TQT_NO_NETWORK
#endif
#ifndef TQT_MODULE_CANVAS
# define TQT_NO_CANVAS
#endif
#ifndef TQT_MODULE_TABLE
#define TQT_NO_TABLE
#endif
#ifndef TQT_MODULE_XML
# define TQT_NO_XML
#endif
#ifndef TQT_MODULE_OPENGL
# define TQT_NO_OPENGL
#endif
#if !defined(TQT_MODULE_SQL)
# define TQT_NO_SQL
#endif

#if defined(TQ_WS_MAC9)
//No need for menu merging
#  ifndef TQMAC_QMENUBAR_NO_MERGE
#    define TQMAC_QMENUBAR_NO_MERGE
#  endif
//Mac9 does not use quartz
#  ifndef TQMAC_NO_QUARTZ
#    define TQMAC_NO_QUARTZ
#  endif
#  ifndef TQMAC_QMENUBAR_NO_EVENT
#    define TQMAC_QMENUBAR_NO_EVENT
#  endif
#endif
#if defined(TQ_WS_MACX) //for no nobody uses quartz, just putting in first level hooks
#  ifndef TQMAC_NO_QUARTZ
#    define TQMAC_NO_QUARTZ
#  endif
#  ifndef TQMAC_QMENUBAR_NO_EVENT
#    define TQMAC_QMENUBAR_NO_EVENT
#  endif
#endif

#if !defined(TQ_WS_QWS) && !defined(TQT_NO_COP)
#  define TQT_NO_COP
#endif

#ifndef QT_H
#include "ntqfeatures.h"
#endif /* QT_H */


//
// Create TQt DLL if QT_DLL is defined (Windows only)
// or QT_SHARED is defined (Kylix only)
//

#if defined(Q_OS_WIN)
#  if defined(QT_NODLL)
#    undef QT_MAKEDLL
#    undef QT_DLL
#  elif defined(QT_MAKEDLL)	/* create a TQt DLL library */
#    if defined(QT_DLL)
#      undef QT_DLL
#    endif
#    define TQ_EXPORT  __declspec(dllexport)
#    define Q_TEMPLATEDLL
#    define Q_TEMPLATE_EXTERN
#    undef  TQ_DISABLE_COPY	/* avoid unresolved externals */
#  elif defined(QT_DLL)		/* use a TQt DLL library */
#    define TQ_EXPORT  __declspec(dllimport)
#    define Q_TEMPLATEDLL
#    ifndef Q_TEMPLATE_EXTERN
#      if defined(Q_CC_MSVC_NET)
#        define Q_TEMPLATE_EXTERN extern
#      else
#        define Q_TEMPLATE_EXTERN
#      endif
#    endif
#    undef  TQ_DISABLE_COPY	/* avoid unresolved externals */
#  endif
#elif defined(Q_CC_GNU) && __GNUC__ - 0 >= 4
#    define TQ_EXPORT __attribute__((visibility("default")))
#    undef QT_MAKEDLL		/* ignore these for other platforms */
#    undef QT_DLL
#else
#  undef QT_MAKEDLL		/* ignore these for other platforms */
#  undef QT_DLL
#endif

#ifndef TQ_EXPORT
#  define TQ_EXPORT
#endif


//
// Some platform specific stuff
//

#if defined(TQ_WS_WIN)
extern TQ_EXPORT bool qt_winunicode;
#endif


//
// System information
//

TQ_EXPORT const char *tqVersion();
TQ_EXPORT bool tqSysInfo( int *wordSize, bool *bigEndian );
TQ_EXPORT bool tqSharedBuild();
#if defined(Q_OS_MAC)
int qMacVersion();
#elif defined(TQ_WS_WIN)
TQ_EXPORT int qWinVersion();
#if defined(UNICODE)
#define QT_WA( uni, ansi ) if ( qt_winunicode ) { uni } else { ansi }
#define QT_WA_INLINE( uni, ansi ) ( qt_winunicode ? uni : ansi )
#else
#define QT_WA( uni, ansi ) ansi
#define QT_WA_INLINE( uni, ansi ) ansi
#endif
#endif

#ifdef Q_OS_TEMP
#ifdef QT_WA
#undef QT_WA
#undef QT_WA_INLINE
#endif
#define QT_WA( uni, ansi ) uni
#define QT_WA_INLINE( uni, ansi ) ( uni )
#endif

#ifndef TQ_INLINE_TEMPLATES
#  define TQ_INLINE_TEMPLATES
#endif

#ifndef TQ_TYPENAME
#  define TQ_TYPENAME typename
#endif

//
// Use to avoid "unused parameter" warnings
//
#define Q_UNUSED(x) (void)x;

//
// Debugging and error handling
//

#if !defined(TQT_NO_CHECK)
#  define QT_CHECK_STATE			// check state of objects etc.
#  define QT_CHECK_RANGE			// check range of indexes etc.
#  define QT_CHECK_NULL				// check null pointers
#  define QT_CHECK_MATH				// check math functions
#endif

#if !defined(TQT_NO_DEBUG) && !defined(QT_DEBUG)
#  define QT_DEBUG				// display debug messages
#  if !defined(TQT_NO_COMPAT)			// compatibility with TQt 2
#    if !defined(NO_DEBUG) && !defined(DEBUG)
#      if !defined(Q_OS_MACX)			// clash with MacOS X headers
#        define DEBUG
#      endif
#    endif
#  endif
#endif


TQ_EXPORT void tqDebug( const TQString& );     // print debug message
TQ_EXPORT void tqDebug( const TQCString& );    // print debug message
TQ_EXPORT void tqDebug( const char *, ... )    // print debug message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

TQ_EXPORT void tqWarning( const TQString& );   // print warning message
TQ_EXPORT void tqWarning( const TQCString& );  // print warning message
TQ_EXPORT void tqWarning( const char *, ... )  // print warning message
#if defined(Q_CC_GNU) && !defined(__INSURE__)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

TQ_EXPORT void tqFatal( const TQString& );     // print fatal message and exit
TQ_EXPORT void tqFatal( const TQCString& );    // print fatal message and exit
TQ_EXPORT void tqFatal( const char *, ... )    // print fatal message and exit
#if defined(Q_CC_GNU)
    __attribute__ ((format (printf, 1, 2)))
#endif
;

TQ_EXPORT void tqSystemWarning( const char *, int code = -1 );


#if !defined(Q_ASSERT)
#  if defined(QT_CHECK_STATE)
#    if defined(QT_FATAL_ASSERT)
#      define Q_ASSERT(x)  ((x) ? (void)0 : tqFatal("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#    else
#      define Q_ASSERT(x)  ((x) ? (void)0 : tqWarning("ASSERT: \"%s\" in %s (%d)",#x,__FILE__,__LINE__))
#    endif
#  else
#    define Q_ASSERT(x)
#  endif
#endif

#if !defined(TQT_NO_COMPAT)			// compatibility with TQt 2
#  if !defined(ASSERT)
#    if !defined(Q_OS_TEMP)
#      define ASSERT(x) Q_ASSERT(x)
#    endif
#  endif
#endif // TQT_NO_COMPAT


TQ_EXPORT bool tqt_check_pointer( bool c, const char *, int );

#if defined(QT_CHECK_NULL)
#  define TQ_CHECK_PTR(p) (tqt_check_pointer((p)==0,__FILE__,__LINE__))
#else
#  define TQ_CHECK_PTR(p)
#endif

enum TQtMsgType { TQtDebugMsg, TQtWarningMsg, TQtFatalMsg };

typedef void (*TQtMsgHandler)(TQtMsgType, const char *);
TQ_EXPORT TQtMsgHandler qInstallMsgHandler( TQtMsgHandler );

#if !defined(TQT_NO_COMPAT)			// compatibility with TQt 2
typedef TQtMsgHandler msg_handler;
#endif // TQT_NO_COMPAT

TQ_EXPORT void tqSuppressObsoleteWarnings( bool = true );

TQ_EXPORT void tqObsolete( const char *obj, const char *oldfunc,
		   const char *newfunc );
TQ_EXPORT void tqObsolete( const char *obj, const char *oldfunc );
TQ_EXPORT void tqObsolete( const char *message );


//
// Install paths from configure
//

TQ_EXPORT const char *tqInstallPath();
TQ_EXPORT const char *tqInstallPathDocs();
TQ_EXPORT const char *tqInstallPathHeaders();
TQ_EXPORT const char *tqInstallPathLibs();
TQ_EXPORT const char *tqInstallPathBins();
TQ_EXPORT const char *tqInstallPathPlugins();
TQ_EXPORT const char *tqInstallPathData();
TQ_EXPORT const char *tqInstallPathTranslations();
TQ_EXPORT const char *tqInstallPathSysconf();
TQ_EXPORT const char *tqInstallPathShare();

#endif /* __cplusplus */

#define TQ_DUMMY_COMPARISON_OPERATOR(C)

#endif /* TQGLOBAL_H */

/*
 Avoid some particularly useless warnings from some stupid compilers.
 To get ALL C++ compiler warnings, define QT_CC_WARNINGS or comment out
 the line "#define TQT_NO_WARNINGS"
*/

#if !defined(QT_CC_WARNINGS)
#  define TQT_NO_WARNINGS
#endif
#if defined(TQT_NO_WARNINGS)
#  if defined(Q_CC_MSVC)
#    pragma warning(disable: 4244) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
#    pragma warning(disable: 4275) // non - DLL-interface classkey 'identifier' used as base for DLL-interface classkey 'identifier'
#    pragma warning(disable: 4514) // unreferenced inline/local function has been removed
#    pragma warning(disable: 4800) // 'type' : forcing value to bool 'true' or 'false' (performance warning)
#    pragma warning(disable: 4097) // typedef-name 'identifier1' used as synonym for class-name 'identifier2'
#    pragma warning(disable: 4706) // assignment within conditional expression
#    pragma warning(disable: 4786) // truncating debug info after 255 characters
#    pragma warning(disable: 4660) // template-class specialization 'identifier' is already instantiated
#    pragma warning(disable: 4355) // 'this' : used in base member initializer list
#    pragma warning(disable: 4231) // nonstandard extension used : 'extern' before template explicit instantiation
#    pragma warning(disable: 4710) // function not inlined
#  endif
#endif

