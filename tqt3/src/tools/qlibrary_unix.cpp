/****************************************************************************
**
** Implementation of TQLibraryPrivate class
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
#include "private/qlibrary_p.h"

#ifndef TQT_NO_LIBRARY

#if defined(QT_AOUT_UNDERSCORE)
#include <string.h>
#endif

/*
  The platform dependent implementations of
  - loadLibrary
  - freeLibrary
  - resolveSymbol

  It's not too hard to guess what the functions do.
*/

#include <dlfcn.h>
#ifndef DL_PREFIX //for mac dlcompat
#  define DL_PREFIX(x) x
#endif

bool TQLibraryPrivate::loadLibrary()
{
    if ( pHnd )
	return true;

    TQString filename = library->library();

    pHnd = DL_PREFIX(dlopen)( filename.latin1(), RTLD_LAZY );
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
    if ( !pHnd )
	tqWarning( "%s", DL_PREFIX(dlerror)() );
#endif
    return pHnd != 0;
}

bool TQLibraryPrivate::freeLibrary()
{
    if ( !pHnd )
	return true;

    if ( DL_PREFIX(dlclose)( pHnd ) ) {
#if defined(QT_DEBUG) || defined(QT_DEBUG_COMPONENT)
	tqWarning( "%s", DL_PREFIX(dlerror)() );
#endif
	return false;
    }

    pHnd = 0;
    return true;
}

void* TQLibraryPrivate::resolveSymbol( const char* symbol )
{
    if ( !pHnd )
	return 0;

#if defined(QT_AOUT_UNDERSCORE)
    // older a.out systems add an underscore in front of symbols
    char* undrscr_symbol = new char[strlen(symbol)+2];
    undrscr_symbol[0] = '_';
    strcpy(undrscr_symbol+1, symbol);
    void* address = DL_PREFIX(dlsym)( pHnd, undrscr_symbol );
    delete [] undrscr_symbol;
#else
    void* address = DL_PREFIX(dlsym)( pHnd, symbol );
#endif
#if defined(QT_DEBUG_COMPONENT)
    const char* error = DL_PREFIX(dlerror)();
    if ( error )
	tqWarning( "%s", error );
#endif
    return address;
}

#endif
