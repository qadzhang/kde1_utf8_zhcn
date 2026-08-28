/****************************************************************************
**
** Implementation of TQLibrary class
**
** Created : 000101
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#include "qplatformdefs.h"
#include <private/qlibrary_p.h>
#include <ntqstringlist.h>
#include <ntqfile.h>

#ifndef TQT_NO_LIBRARY

// uncomment this to get error messages
//#define QT_DEBUG_COMPONENT 1
// uncomment this to get error and success messages
//#define QT_DEBUG_COMPONENT 2

#ifndef QT_DEBUG_COMPONENT
# if defined(QT_DEBUG)
#  define QT_DEBUG_COMPONENT 1
# endif
#endif

#if (defined(TQ_WS_WIN) && !defined(QT_MAKEDLL)) \
    || (defined(Q_OS_FREEBSD) && defined(Q_CC_INTEL))
#define TQT_NO_LIBRARY_UNLOAD
#endif

TQLibraryPrivate::TQLibraryPrivate( TQLibrary *lib )
    : pHnd( 0 ), library( lib )
{
}


/*!
    \class TQLibrary ntqlibrary.h
    \reentrant
    \brief The TQLibrary class provides a wrapper for handling shared libraries.

    \mainclass
    \ingroup plugins

    An instance of a TQLibrary object can handle a single shared
    library and provide access to the functionality in the library in
    a platform independent way. If the library is a component server,
    TQLibrary provides access to the exported component and can
    directly query this component for interfaces.

    TQLibrary ensures that the shared library is loaded and stays in
    memory whilst it is in use. TQLibrary can also unload the library
    on destruction and release unused resources.

    A typical use of TQLibrary is to resolve an exported symbol in a
    shared object, and to call the function that this symbol
    represents. This is called "explicit linking" in contrast to
    "implicit linking", which is done by the link step in the build
    process when linking an executable against a library.

    The following code snippet loads a library, resolves the symbol
    "mysymbol", and calls the function if everything succeeded. If
    something went wrong, e.g. the library file does not exist or the
    symbol is not defined, the function pointer will be 0 and won't be
    called. When the TQLibrary object is destroyed the library will be
    unloaded, making all references to memory allocated in the library
    invalid.

    \code
    typedef void (*MyPrototype)();
    MyPrototype myFunction;

    TQLibrary myLib( "mylib" );
    myFunction = (MyPrototype) myLib.resolve( "mysymbol" );
    if ( myFunction ) {
	myFunction();
    }
    \endcode
*/

/*!
    Creates a TQLibrary object for the shared library \a filename. The
    library will be unloaded in the destructor.

    Note that \a filename does not need to include the (platform specific)
    file extension, so calling
    \code
    TQLibrary lib( "mylib" );
    \endcode
    is equivalent to calling
    \code
    TQLibrary lib( "mylib.dll" );
    \endcode
    on Windows, and
    \code
    TQLibrary lib( "libmylib.so" );
    \endcode
    on Unix. Specifying the extension is not recommended, since
    doing so introduces a platform dependency.

    If \a filename does not include a path, the library loader will
    look for the file in the platform specific search paths.

    \sa load() unload(), setAutoUnload()
*/
TQLibrary::TQLibrary( const TQString& filename )
    : libfile( filename ), aunload( true )
{
    libfile.replace( '\\', '/' );
    d = new TQLibraryPrivate( this );
}

/*!
    Deletes the TQLibrary object.

    The library will be unloaded if autoUnload() is true (the
    default), otherwise it stays in memory until the application
    exits.

    \sa unload(), setAutoUnload()
*/
TQLibrary::~TQLibrary()
{
    if ( autoUnload() )
	unload();

    delete d;
}

/*!
    Returns the address of the exported symbol \a symb. The library is
    loaded if necessary. The function returns 0 if the symbol could
    not be resolved or the library could not be loaded.

    \code
    typedef int (*avgProc)( int, int );

    avgProc avg = (avgProc) library->resolve( "avg" );
    if ( avg )
	return avg( 5, 8 );
    else
	return -1;
    \endcode

    The symbol must be exported as a C-function from the library. This
    requires the \c {extern "C"} notation if the library is compiled
    with a C++ compiler. On Windows you also have to explicitly export
    the function from the DLL using the \c {__declspec(dllexport)}
    compiler directive.

    \code
    extern "C" MY_EXPORT_MACRO int avg(int a, int b)
    {
	return (a + b) / 2;
    }
    \endcode

    with \c MY_EXPORT defined as

    \code
    #ifdef TQ_WS_WIN
    # define MY_EXPORT __declspec(dllexport)
    #else
    # define MY_EXPORT
    #endif
    \endcode

    On Darwin and Mac OS X this function uses code from dlcompat, part of the
    OpenDarwin project.

    \legalese

    Copyright (c) 2002 Jorge Acereda and Peter O'Gorman

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
void *TQLibrary::resolve( const char* symb )
{
    if ( !d->pHnd )
	load();
    if ( !d->pHnd )
	return 0;

    void *address = d->resolveSymbol( symb );

    return address;
}

/*!
    \overload

    Loads the library \a filename and returns the address of the
    exported symbol \a symb. Note that like the constructor, \a
    filename does not need to include the (platform specific) file
    extension. The library remains loaded until the process exits.

    The function returns 0 if the symbol could not be resolved or the
    library could not be loaded.

    This function is useful only if you want to resolve a single
    symbol, e.g. a function pointer from a specific library once:

    \code
    typedef void (*FunctionType)();
    static FunctionType *ptrFunction = 0;
    static bool triedResolve = false;
    if ( !ptrFunction && !triedResolve )
	ptrFunction = TQLibrary::resolve( "mylib", "mysymb" );

    if ( ptrFunction )
	ptrFunction();
    else
	...
    \endcode

    If you want to resolve multiple symbols, use a TQLibrary object and
    call the non-static version of resolve().

    \sa resolve()
*/
void *TQLibrary::resolve( const TQString &filename, const char *symb )
{
    TQLibrary lib( filename );
    lib.setAutoUnload( false );
    return lib.resolve( symb );
}

/*!
    Returns true if the library is loaded; otherwise returns false.

    \sa unload()
*/
bool TQLibrary::isLoaded() const
{
    return d->pHnd != 0;
}

/*!
    Loads the library. Since resolve() always calls this function
    before resolving any symbols it is not necessary to call it
    explicitly. In some situations you might want the library loaded
    in advance, in which case you would use this function.

    On Darwin and Mac OS X this function uses code from dlcompat, part of the
    OpenDarwin project.

    \legalese

    Copyright (c) 2002 Jorge Acereda and Peter O'Gorman

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
bool TQLibrary::load()
{
    if (libfile.isEmpty())
        return false;
    return d->loadLibrary();
}

/*!
    Unloads the library and returns true if the library could be
    unloaded; otherwise returns false.

    This function is called by the destructor if autoUnload() is
    enabled.

    \sa resolve()
*/
bool TQLibrary::unload()
{
    if ( !d->pHnd )
	return true;

#if !defined(TQT_NO_LIBRARY_UNLOAD)
    if ( !d->freeLibrary() ) {
# if defined(QT_DEBUG_COMPONENT)
	tqWarning( "%s could not be unloaded", (const char*) TQFile::encodeName(library()) );
# endif
	return false;
    }

# if defined(QT_DEBUG_COMPONENT) && QT_DEBUG_COMPONENT == 2
    tqWarning( "%s has been unloaded", (const char*) TQFile::encodeName(library()) );
# endif
    d->pHnd = 0;
#endif
    return true;
}

/*!
    Returns true if the library will be automatically unloaded when
    this wrapper object is destructed; otherwise returns false. The
    default is true.

    \sa setAutoUnload()
*/
bool TQLibrary::autoUnload() const
{
    return (bool)aunload;
}

/*!
    If \a enabled is true (the default), the wrapper object is set to
    automatically unload the library upon destruction. If \a enabled
    is false, the wrapper object is not unloaded unless you explicitly
    call unload().

    \sa autoUnload()
*/
void TQLibrary::setAutoUnload( bool enabled )
{
    aunload = enabled;
}

/*!
    Returns the filename of the shared library this TQLibrary object
    handles, including the platform specific file extension.

    For example:
    \code
    TQLibrary lib( "mylib" );
    TQString str = lib.library();
    \endcode
    will set \e str to "mylib.dll" on Windows, and "libmylib.so" on Linux.
*/
TQString TQLibrary::library() const
{
    if ( libfile.isEmpty() )
	return libfile;

    TQString filename = libfile;

#if defined(TQ_WS_WIN)
    if ( filename.findRev( '.' ) <= filename.findRev( '/' ) )
	filename += ".dll";
#else
    TQStringList filters = "";
#ifdef Q_OS_MACX
    filters << ".so";
    filters << ".bundle";
    filters << ".dylib"; //the last one is also the default one..
#else
    filters << ".so";
#endif
    for(TQStringList::Iterator it = filters.begin(); true; ) {
	TQString filter = (*it);
	++it;

	if(TQFile::exists(filename + filter)) {
	    filename += filter;
	    break;
	} else if(!filter.isEmpty()) {
	    TQString tmpfilename = filename;
	    const int x = tmpfilename.findRev( "/" );
	    if ( x != -1 ) {
		TQString path = tmpfilename.left( x + 1 );
		TQString file = tmpfilename.right( tmpfilename.length() - x - 1 );
		tmpfilename = TQString( "%1lib%2" ).arg( path ).arg( file );
	    } else {
		tmpfilename = TQString( "lib%1" ).arg( filename );
	    }
	    if ( !filename.contains(".so") )
	    tmpfilename += filter;
	    if(TQFile::exists(tmpfilename) || it == filters.end()) {
		filename = tmpfilename;
		break;
	    }
	}
    }
#endif
    return filename;
}
#endif //TQT_NO_LIBRARY
