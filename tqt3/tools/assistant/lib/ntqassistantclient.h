/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQAssistantClient library.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQASSISTANTCLIENT_H
#define TQASSISTANTCLIENT_H

#include <ntqobject.h>

class TQSocket;
class TQProcess;

class TQ_EXPORT TQAssistantClient : public TQObject
{
    TQ_OBJECT
    TQ_PROPERTY( bool open READ isOpen )

public:
    TQAssistantClient( const TQString &path, TQObject *parent = 0, const char *name = 0 );
    ~TQAssistantClient();

    bool isOpen() const;

    void setArguments( const TQStringList &args );

public slots:
    virtual void openAssistant();
    virtual void closeAssistant();
    virtual void showPage( const TQString &page );

signals:
    void assistantOpened();
    void assistantClosed();
    void error( const TQString &msg );

private slots:
    void socketConnected();
    void socketConnectionClosed();
    void readPort();
    void socketError( int );
    void readStdError();

private:
    TQSocket *socket;
    TQProcess *proc;
    TQ_UINT16 port;
    TQString host, assistantCommand, pageBuffer;
    bool opened;
};

#endif
