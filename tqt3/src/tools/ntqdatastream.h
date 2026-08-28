/****************************************************************************
**
** Definition of TQDataStream class
**
** Created : 930831
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

#ifndef TQDATASTREAM_H
#define TQDATASTREAM_H

#ifndef QT_H
#include "ntqiodevice.h"
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_DATASTREAM
class TQ_EXPORT TQDataStream				// data stream class
{
public:
    TQDataStream();
    TQDataStream( TQIODevice * );
    TQDataStream( TQByteArray, int mode );
    virtual ~TQDataStream();

    TQIODevice	*device() const;
    void	 setDevice( TQIODevice * );
    void	 unsetDevice();

    bool	 atEnd() const;
    bool	 eof() const;

    enum ByteOrder { BigEndian, LittleEndian };
    int		 byteOrder()	const;
    void	 setByteOrder( int );

    bool	 isPrintableData() const;
    void	 setPrintableData( bool );

    int		 version() const;
    void	 setVersion( int );

    TQDataStream &operator>>( TQ_INT8 &i );
    TQDataStream &operator>>( TQ_UINT8 &i );
    TQDataStream &operator>>( TQ_INT16 &i );
    TQDataStream &operator>>( TQ_UINT16 &i );
    TQDataStream &operator>>( TQ_INT32 &i );
    TQDataStream &operator>>( TQ_UINT32 &i );
    TQDataStream &operator>>( TQ_INT64 &i );
    TQDataStream &operator>>( TQ_UINT64 &i );
#if !defined(Q_OS_WIN64)
    TQDataStream &operator>>( TQ_LONG &i );
    TQDataStream &operator>>( TQ_ULONG &i );
#endif

    TQDataStream &operator>>( float &f );
    TQDataStream &operator>>( double &f );
    TQDataStream &operator>>( char *&str );

    TQDataStream &operator<<( TQ_INT8 i );
    TQDataStream &operator<<( TQ_UINT8 i );
    TQDataStream &operator<<( TQ_INT16 i );
    TQDataStream &operator<<( TQ_UINT16 i );
    TQDataStream &operator<<( TQ_INT32 i );
    TQDataStream &operator<<( TQ_UINT32 i );
    TQDataStream &operator<<( TQ_INT64 i );
    TQDataStream &operator<<( TQ_UINT64 i );
#if !defined(Q_OS_WIN64)
    TQDataStream &operator<<( TQ_LONG i );
    TQDataStream &operator<<( TQ_ULONG i );
#endif
    TQDataStream &operator<<( float f );
    TQDataStream &operator<<( double f );
    TQDataStream &operator<<( const char *str );

    TQDataStream &readBytes( char *&, uint &len );
    TQDataStream &readRawBytes( char *, uint len );

    TQDataStream &writeBytes( const char *, uint len );
    TQDataStream &writeRawBytes( const char *, uint len );

private:
    TQIODevice	*dev;
    bool	 owndev;
    int		 byteorder;
    bool	 printable;
    bool	 noswap;
    int		 ver;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQDataStream( const TQDataStream & );
    TQDataStream &operator=( const TQDataStream & );
#endif
};


/*****************************************************************************
  TQDataStream inline functions
 *****************************************************************************/

inline TQIODevice *TQDataStream::device() const
{ return dev; }

inline bool TQDataStream::atEnd() const
{ return dev ? dev->atEnd() : true; }

inline bool TQDataStream::eof() const
{ return atEnd(); }

inline int TQDataStream::byteOrder() const
{ return byteorder; }

inline bool TQDataStream::isPrintableData() const
{ return printable; }

inline void TQDataStream::setPrintableData( bool p )
{ printable = p; }

inline int TQDataStream::version() const
{ return ver; }

inline void TQDataStream::setVersion( int v )
{ ver = v; }

inline TQDataStream &TQDataStream::operator>>( TQ_UINT8 &i )
{ return *this >> (TQ_INT8&)i; }

inline TQDataStream &TQDataStream::operator>>( TQ_UINT16 &i )
{ return *this >> (TQ_INT16&)i; }

inline TQDataStream &TQDataStream::operator>>( TQ_UINT32 &i )
{ return *this >> (TQ_INT32&)i; }

inline TQDataStream &TQDataStream::operator>>( TQ_UINT64 &i )
{ return *this >> (TQ_INT64&)i; }

#if !defined(Q_OS_WIN64)
inline TQDataStream &TQDataStream::operator>>( TQ_ULONG &i )
{ return *this >> (TQ_LONG&)i; }
#endif

inline TQDataStream &TQDataStream::operator<<( TQ_UINT8 i )
{ return *this << (TQ_INT8)i; }

inline TQDataStream &TQDataStream::operator<<( TQ_UINT16 i )
{ return *this << (TQ_INT16)i; }

inline TQDataStream &TQDataStream::operator<<( TQ_UINT32 i )
{ return *this << (TQ_INT32)i; }

inline TQDataStream &TQDataStream::operator<<( TQ_UINT64 i )
{ return *this << (TQ_INT64)i; }

#if !defined(Q_OS_WIN64)
inline TQDataStream &TQDataStream::operator<<( TQ_ULONG i )
{ return *this << (TQ_LONG)i; }
#endif

#endif // TQT_NO_DATASTREAM
#endif // TQDATASTREAM_H
