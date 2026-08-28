/****************************************************************************
**
** Definition of TQCursor class
**
** Created : 940219
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQCURSOR_H
#define TQCURSOR_H

#ifndef QT_H
#include "ntqpoint.h"
#include "ntqshared.h"
#endif // QT_H

/*
  ### The fake cursor has to go first with old qdoc.
*/
#ifdef TQT_NO_CURSOR

class TQ_EXPORT TQCursor : public TQt
{
public:
    static TQPoint pos();
    static void	  setPos( int x, int y );
    static void	  setPos( const TQPoint & );
private:
    TQCursor();
};

#endif // TQT_NO_CURSOR

#ifndef TQT_NO_CURSOR

struct TQCursorData;


class TQ_EXPORT TQCursor : public TQt
{
public:
    TQCursor();				// create default arrow cursor
    TQCursor( int shape );
    TQCursor( const TQBitmap &bitmap, const TQBitmap &mask,
	     int hotX=-1, int hotY=-1 );
    TQCursor( const TQPixmap &pixmap,
	     int hotX=-1, int hotY=-1 );
    TQCursor( const TQCursor & );
   ~TQCursor();
    TQCursor &operator=( const TQCursor & );

    int		  shape()   const;
    void	  setShape( int );

    const TQBitmap *bitmap() const;
    const TQBitmap *mask()   const;
    TQPoint	  hotSpot() const;

#if defined(TQ_WS_WIN)
    HCURSOR	  handle()  const;
    TQCursor( HCURSOR );
#elif defined(TQ_WS_X11)
    HANDLE	  handle()  const;
    TQCursor( HANDLE );
#elif defined(TQ_WS_MAC)
    HANDLE handle() const;
#elif defined(TQ_WS_QWS)
    HANDLE	  handle()  const;
#endif

    static TQPoint pos();
    static void	  setPos( int x, int y );
    static void	  setPos( const TQPoint & );

    static void	  initialize();
    static void	  cleanup();

#if defined(TQ_WS_X11)
    static int 	  x11Screen();
#endif
private:
    void	  setBitmap( const TQBitmap &bitmap, const TQBitmap &mask,
				 int hotX, int hotY );
    void	  update() const;
    TQCursorData	 *data;
    TQCursor	 *find_cur(int);
#if defined(TQ_WS_MAC)
    friend void tqt_mac_set_cursor(const TQCursor *c, const Point *p);
#endif
};


/*****************************************************************************
  TQCursor stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQCursor & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQCursor & );
#endif
#endif // TQT_NO_CURSOR


inline void TQCursor::setPos( const TQPoint &p )
{
    setPos( p.x(), p.y() );
}

#endif // TQCURSOR_H
