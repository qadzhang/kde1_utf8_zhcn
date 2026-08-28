/****************************************************************************
**
** Definition of TQGLColormap class
**
** Created : 20010326
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the opengl module of the TQt GUI Toolkit.
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

#ifndef TQGLCOLORMAP_H
#define TQGLCOLORMAP_H

#ifndef QT_H
#include "ntqcolor.h"
#include "ntqmemarray.h"
#include "ntqshared.h"
#endif // QT_H

#if !defined( TQT_MODULE_OPENGL ) || defined( QT_LICENSE_PROFESSIONAL )
#define TQM_EXPORT_OPENGL
#else
#define TQM_EXPORT_OPENGL TQ_EXPORT
#endif

class TQWidget;
class TQM_EXPORT_OPENGL TQGLColormap
{
public:
    TQGLColormap();
    TQGLColormap( const TQGLColormap & );
    ~TQGLColormap();
    
    TQGLColormap &operator=( const TQGLColormap & );
    
    bool   isEmpty() const;
    int    size() const;
    void   detach();

    void   setEntries( int count, const TQRgb * colors, int base = 0 );
    void   setEntry( int idx, TQRgb color );
    void   setEntry( int idx, const TQColor & color );
    TQRgb   entryRgb( int idx ) const;
    TQColor entryColor( int idx ) const;
    int    find( TQRgb color ) const;
    int    findNearest( TQRgb color ) const;
    
private:
    class Private : public TQShared
    {
    public:
	Private() {
	    cells.resize( 256 ); // ### hardcoded to 256 entries for now
	    cmapHandle = 0;
	}

	~Private() {
	}

	TQMemArray<TQRgb> cells;
	TQt::HANDLE      cmapHandle;
    };
    
    Private * d;

    friend class TQGLWidget;
};

#endif
