/****************************************************************************
**
** Definition of TQFile class
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

#ifndef TQFILE_H
#define TQFILE_H

#ifndef QT_H
#include "ntqiodevice.h"
#include "ntqstring.h"
#include <stdio.h>
#endif // QT_H

class TQDir;
class TQFilePrivate;

class TQ_EXPORT TQFile : public TQIODevice			// file I/O device class
{
public:
    TQFile();
    TQFile( const TQString &name );
   ~TQFile();

    TQString	name()	const;
    void	setName( const TQString &name );

    typedef TQCString (*EncoderFn)( const TQString &fileName );
    typedef TQString (*DecoderFn)( const TQCString &localfileName );
    static TQCString encodeName( const TQString &fileName );
    static TQString decodeName( const TQCString &localFileName );
    static void setEncodingFunction( EncoderFn );
    static void setDecodingFunction( DecoderFn );

    bool	exists()   const;
    static bool exists( const TQString &fileName );

    bool	remove();
    static bool remove( const TQString &fileName );

    bool	open( int );
    bool	open( int, FILE * );
    bool	open( int, int );
    void	close();
    void	flush();

    Offset	size() const;
    Offset	at() const;
    bool	at( Offset );
    bool	atEnd() const;

    TQ_LONG	readBlock( char *data, TQ_ULONG len );
    TQ_LONG	writeBlock( const char *data, TQ_ULONG len );
    TQ_LONG	writeBlock( const TQByteArray& data )
		      { return TQIODevice::writeBlock(data); }
    TQ_LONG	readLine( char *data, TQ_ULONG maxlen );
    TQ_LONG	readLine( TQString &, TQ_ULONG maxlen );

    int		getch();
    int		putch( int );
    int		ungetch( int );

    int		handle() const;

    TQString	errorString() const; // ### TQt 4: move into TQIODevice

protected:
    void	setErrorString( const TQString& ); // ### TQt 4: move into TQIODevice
    TQString	fn;
    FILE       *fh;
    int		fd;
    Offset	length;
    bool	ext_f;
    TQFilePrivate *d; // ### TQt 4: make private

private:
    void	init();
    void	setErrorStringErrno( int );
    TQCString	ungetchBuffer;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQFile( const TQFile & );
    TQFile &operator=( const TQFile & );
#endif
};


inline TQString TQFile::name() const
{ return fn; }

inline TQIODevice::Offset TQFile::at() const
{ return ioIndex; }


#endif // TQFILE_H
