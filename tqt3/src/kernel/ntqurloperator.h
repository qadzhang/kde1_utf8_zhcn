/****************************************************************************
**
** Definition of TQUrlOperator class
**
** Created : 950429
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

#ifndef TQURLOPERATOR_H
#define TQURLOPERATOR_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqurl.h"
#include "ntqptrlist.h"
#include "ntqnetworkprotocol.h"
#include "ntqstringlist.h" // TQString->TQStringList conversion
#endif // QT_H

#ifndef TQT_NO_NETWORKPROTOCOL

class TQUrlInfo;
class TQUrlOperatorPrivate;

class TQ_EXPORT TQUrlOperator : public TQObject, public TQUrl
{
    friend class TQNetworkProtocol;

    TQ_OBJECT

public:
    TQUrlOperator();
    TQUrlOperator( const TQString &urL );
    TQUrlOperator( const TQUrlOperator& url );
    TQUrlOperator( const TQUrlOperator& url, const TQString& relUrl, bool checkSlash = false );
    virtual ~TQUrlOperator();

    virtual void setPath( const TQString& path );
    virtual bool cdUp();

    virtual const TQNetworkOperation *listChildren();
    virtual const TQNetworkOperation *mkdir( const TQString &dirname );
    virtual const TQNetworkOperation *remove( const TQString &filename );
    virtual const TQNetworkOperation *rename( const TQString &oldname, const TQString &newname );
    virtual const TQNetworkOperation *get( const TQString &location = TQString::null );
    virtual const TQNetworkOperation *put( const TQByteArray &data, const TQString &location = TQString::null  );
    virtual TQPtrList<TQNetworkOperation> copy( const TQString &from, const TQString &to, bool move = false, bool toPath = true );
    virtual void copy( const TQStringList &files, const TQString &dest, bool move = false );
    virtual bool isDir( bool *ok = 0 );

    virtual void setNameFilter( const TQString &nameFilter );
    TQString nameFilter() const;

    virtual TQUrlInfo info( const TQString &entry ) const;

    TQUrlOperator& operator=( const TQUrlOperator &url );
    TQUrlOperator& operator=( const TQString &url );

    virtual void stop();

signals:
    void newChildren( const TQValueList<TQUrlInfo> &, TQNetworkOperation *res );
    void finished( TQNetworkOperation *res );
    void start( TQNetworkOperation *res );
    void createdDirectory( const TQUrlInfo &, TQNetworkOperation *res );
    void removed( TQNetworkOperation *res );
    void itemChanged( TQNetworkOperation *res );
    void data( const TQByteArray &, TQNetworkOperation *res );
    void dataTransferProgress( int bytesDone, int bytesTotal, TQNetworkOperation *res );
    void startedNextCopy( const TQPtrList<TQNetworkOperation> &lst );
    void connectionStateChanged( int state, const TQString &data );

protected:
    void reset();
    bool parse( const TQString& url );
    virtual bool checkValid();
    virtual void clearEntries();
    void getNetworkProtocol();
    void deleteNetworkProtocol();

private slots:
    const TQNetworkOperation *startOperation( TQNetworkOperation *op );
    void copyGotData( const TQByteArray &data, TQNetworkOperation *op );
    void continueCopy( TQNetworkOperation *op );
    void finishedCopy();
    void addEntry( const TQValueList<TQUrlInfo> &i );
    void slotItemChanged( TQNetworkOperation *op );

private:
    void deleteOperation( TQNetworkOperation *op );

    TQUrlOperatorPrivate *d;
};

#endif // TQT_NO_NETWORKPROTOCOL

#endif // TQURLOPERATOR_H
