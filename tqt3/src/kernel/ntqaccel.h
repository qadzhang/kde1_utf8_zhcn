/****************************************************************************
**
** Definition of TQAccel class
**
** Created : 950419
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

#ifndef TQACCEL_H
#define TQACCEL_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqkeysequence.h"
#endif // QT_H

#ifndef TQT_NO_ACCEL

class TQAccelPrivate;

class TQ_EXPORT TQAccel : public TQObject			// accelerator class
{
    TQ_OBJECT
public:
    TQAccel( TQWidget *parent, const char *name=0 );
    TQAccel( TQWidget* watch, TQObject *parent, const char *name=0 );
    ~TQAccel();

    bool isEnabled() const;
    void setEnabled( bool );

    uint count() const;

    int insertItem( const TQKeySequence& key, int id=-1);
    void removeItem( int id );
    void clear();

    TQKeySequence key( int id );
    int findKey( const TQKeySequence& key ) const;

    bool isItemEnabled( int id ) const;
    void setItemEnabled( int id, bool enable );

    bool connectItem( int id,  const TQObject *receiver, const char* member );
    bool disconnectItem( int id,  const TQObject *receiver, const char* member );

    void repairEventFilter();

    void setWhatsThis( int id, const TQString& );
    TQString whatsThis( int id ) const;
    void setIgnoreWhatsThis( bool );
    bool ignoreWhatsThis() const;

    static TQKeySequence shortcutKey( const TQString & );
    static TQString keyToString(TQKeySequence k );
    static TQKeySequence stringToKey( const TQString & );

signals:
    void activated( int id );
    void activatedAmbiguously( int id );

protected:
    bool eventFilter( TQObject *, TQEvent * );

private:
    TQAccelPrivate * d;

private:
#if defined(TQ_DISABLE_COPY)
    TQAccel( const TQAccel & );
    TQAccel &operator=( const TQAccel & );
#endif
    friend class TQAccelPrivate;
    friend class TQAccelManager;
};

#endif // TQT_NO_ACCEL
#endif // TQACCEL_H
