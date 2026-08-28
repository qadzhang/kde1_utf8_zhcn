/****************************************************************************
**
** Definition of TQBuffer class
**
** Created : 930812
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

#ifndef TQBUFFER_H
#define TQBUFFER_H

#ifndef QT_H
#include "ntqiodevice.h"
#include "ntqstring.h"
#endif // QT_H


class TQ_EXPORT TQBuffer : public TQIODevice
{
public:
    TQBuffer();
    TQBuffer( TQByteArray );
   ~TQBuffer();

    TQByteArray buffer() const;
    bool  setBuffer( TQByteArray );

    bool  open( int );
    void  close();
    void  flush();

    Offset size() const;
    Offset at() const;
    bool  at( Offset );

    TQ_LONG	  readBlock( char *p, TQ_ULONG );
    TQ_LONG	  writeBlock( const char *p, TQ_ULONG );
    TQ_LONG	  writeBlock( const TQByteArray& data )
	      { return TQIODevice::writeBlock(data); }
    TQ_LONG	  readLine( char *p, TQ_ULONG );

    int	  getch();
    int	  putch( int );
    int	  ungetch( int );

protected:
    TQByteArray a;

private:
    uint  a_len;
    uint  a_inc;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQBuffer( const TQBuffer & );
    TQBuffer &operator=( const TQBuffer & );
#endif
};


inline TQByteArray TQBuffer::buffer() const
{ return a; }

inline TQIODevice::Offset TQBuffer::size() const
{ return (Offset)a.size(); }

inline TQIODevice::Offset TQBuffer::at() const
{ return ioIndex; }


#endif // TQBUFFER_H
