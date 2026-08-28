/****************************************************************************
**
** Definition of some shared interal classes
**
** Created : 010427
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

#ifndef TQINTERNAL_P_H
#define TQINTERNAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of a number of TQt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//
#ifndef QT_H
#include "ntqnamespace.h"
#include "ntqrect.h"
#include "ntqptrlist.h"
#include "ntqcstring.h"
#include "ntqiodevice.h"
#endif // QT_H

class TQWidget;
class TQPainter;
class TQPixmap;

class TQ_EXPORT TQSharedDoubleBuffer
{
public:
    enum DoubleBufferFlags {
	NoFlags         = 0x00,
	InitBG		= 0x01,
	Force		= 0x02,
	Default		= InitBG | Force
    };
    typedef uint DBFlags;

    TQSharedDoubleBuffer( DBFlags f = Default );
    TQSharedDoubleBuffer( TQWidget* widget,
			 int x = 0, int y = 0, int w = -1, int h = -1,
			 DBFlags f = Default );
    TQSharedDoubleBuffer( TQPainter* painter,
			 int x = 0, int y = 0, int w = -1, int h = -1,
			 DBFlags f = Default );
    TQSharedDoubleBuffer( TQWidget *widget, const TQRect &r, DBFlags f = Default );
    TQSharedDoubleBuffer( TQPainter *painter, const TQRect &r, DBFlags f = Default );
    ~TQSharedDoubleBuffer();

    bool begin( TQWidget* widget, int x = 0, int y = 0, int w = -1, int h = -1 );
    bool begin( TQPainter* painter, int x = 0, int y = 0, int w = -1, int h = -1);
    bool begin( TQWidget* widget, const TQRect &r );
    bool begin( TQPainter* painter, const TQRect &r );
    bool end();

    TQPainter* painter() const;

    bool isActive() const;
    bool isBuffered() const;
    void flush();

    static bool isDisabled() { return !dblbufr; }
    static void setDisabled( bool off ) { dblbufr = !off; }

    static void cleanup();

private:
    enum DoubleBufferState {
	Active		= 0x0100,
	BufferActive	= 0x0200,
	ExternalPainter	= 0x0400
    };
    typedef uint DBState;

    TQPixmap *getPixmap();
    void releasePixmap();

    TQWidget *wid;
    int rx, ry, rw, rh;
    DBFlags flags;
    DBState state;

    TQPainter *p, *external_p;
    TQPixmap *pix;

    static bool dblbufr;
};

inline bool TQSharedDoubleBuffer::begin( TQWidget* widget, const TQRect &r )
{ return begin( widget, r.x(), r.y(), r.width(), r.height() ); }

inline bool TQSharedDoubleBuffer::begin( TQPainter *painter, const TQRect &r )
{ return begin( painter, r.x(), r.y(), r.width(), r.height() ); }

inline TQPainter* TQSharedDoubleBuffer::painter() const
{ return p; }

inline bool TQSharedDoubleBuffer::isActive() const
{ return ( state & Active ); }

inline bool TQSharedDoubleBuffer::isBuffered() const
{ return ( state & BufferActive ); }


class TQVirtualDestructor {
public:
    virtual ~TQVirtualDestructor() {}
};

template <class T>
class TQAutoDeleter : public TQVirtualDestructor {
public:
    TQAutoDeleter( T* p ) : ptr( p ) {}
    ~TQAutoDeleter() { delete ptr; }
    T* data() const { return ptr; }
private:
    T* ptr;
};

template <class T>
T* qAutoDeleterData( TQAutoDeleter<T>* ad )
{
    if ( !ad )
	return 0;
    return ad->data();
}

template <class T>
TQAutoDeleter<T>* qAutoDeleter( T* p )
{
    return new TQAutoDeleter<T>( p );
}

class TQ_EXPORT TQMembuf
{
public:
    TQMembuf();
    ~TQMembuf();

    void append( TQByteArray *ba );
    void clear();

    bool consumeBytes( TQ_ULONG nbytes, char *sink );
    TQByteArray readAll();
    bool scanNewline( TQByteArray *store );
    bool canReadLine() const;

    int ungetch( int ch );

    TQIODevice::Offset size() const;

private:

    TQPtrList<TQByteArray> *buf;
    TQIODevice::Offset _size;
    TQIODevice::Offset _index;
};

inline void TQMembuf::append( TQByteArray *ba )
{ buf->append( ba ); _size += ba->size(); }

inline void TQMembuf::clear()
{ buf->clear(); _size=0; _index=0; }

inline TQByteArray TQMembuf::readAll()
{ TQByteArray ba(_size); consumeBytes(_size,ba.data()); return ba; }

inline bool TQMembuf::canReadLine() const
{ return ((TQMembuf*)this)->scanNewline( 0 ); }

inline TQIODevice::Offset TQMembuf::size() const
{ return _size; }

#endif // TQINTERNAL_P_H
