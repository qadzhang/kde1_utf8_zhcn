/****************************************************************************
**
** Definition of TQKeySequence class
**
** Created : 0108007
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

#ifndef TQKEYSEQUENCE_H
#define TQKEYSEQUENCE_H

#ifndef QT_H
#ifndef QT_H
#include "ntqnamespace.h"
#include "ntqstring.h"
#endif // QT_H
#endif

#ifndef TQT_NO_ACCEL

/*****************************************************************************
  TQKeySequence stream functions
 *****************************************************************************/
#ifndef TQT_NO_DATASTREAM
class TQKeySequence;
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQKeySequence & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQKeySequence & );
#endif

class TQKeySequencePrivate;

class TQ_EXPORT TQKeySequence : public TQt
{
public:
    TQKeySequence();
    TQKeySequence( const TQString& key );
    TQKeySequence( int key );
    TQKeySequence( int k1, int k2, int k3 = 0, int k4 = 0 );
    TQKeySequence( const TQKeySequence & );
    ~TQKeySequence();

    uint count() const;
    bool isEmpty() const;
    TQt::SequenceMatch matches( const TQKeySequence & ) const;

    operator TQString() const;
    operator int () const;
    int operator[]( uint ) const;
    TQKeySequence &operator=( const TQKeySequence & );
    bool operator==( const TQKeySequence& ) const;
    bool operator!= ( const TQKeySequence& ) const;

private:
    static int decodeString( const TQString & );
    static TQString encodeString( int );
    int assign( TQString );
    void setKey( int key, int index );

    TQKeySequencePrivate* d;

    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQKeySequence & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQKeySequence & );
    friend class TQAccelManager;
};

#else

class TQ_EXPORT TQKeySequence : public TQt
{
public:
    TQKeySequence() {}
    TQKeySequence( int ) {}
};

#endif //TQT_NO_ACCEL

#endif
