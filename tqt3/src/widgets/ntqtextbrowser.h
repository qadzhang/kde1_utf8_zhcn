/****************************************************************************
**
** Definition of the TQTextBrowser class
**
** Created : 990101
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
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

#ifndef TQTEXTBROWSER_H
#define TQTEXTBROWSER_H

#ifndef QT_H
#include "ntqptrlist.h"
#include "ntqpixmap.h"
#include "ntqcolor.h"
#include "ntqtextedit.h"
#endif // QT_H

#ifndef TQT_NO_TEXTBROWSER

class TQTextBrowserData;

class TQ_EXPORT TQTextBrowser : public TQTextEdit
{
    TQ_OBJECT
    TQ_PROPERTY( TQString source READ source WRITE setSource )
    TQ_OVERRIDE( int undoDepth DESIGNABLE false SCRIPTABLE false )
    TQ_OVERRIDE( bool overwriteMode DESIGNABLE false SCRIPTABLE false )
    TQ_OVERRIDE( bool modified SCRIPTABLE false)
    TQ_OVERRIDE( bool readOnly DESIGNABLE false SCRIPTABLE false )
    TQ_OVERRIDE( bool undoRedoEnabled DESIGNABLE false SCRIPTABLE false )

public:
    TQTextBrowser( TQWidget* parent=0, const char* name=0 );
    ~TQTextBrowser();

    TQString source() const;

public slots:
    virtual void setSource(const TQString& name);
    virtual void backward();
    virtual void forward();
    virtual void home();
    virtual void reload();
    void setText( const TQString &txt ) { setText( txt, TQString::null ); }
    virtual void setText( const TQString &txt, const TQString &context );

signals:
    void backwardAvailable( bool );
    void forwardAvailable( bool );
    void sourceChanged( const TQString& );
    void highlighted( const TQString& );
    void linkClicked( const TQString& );
    void anchorClicked( const TQString&, const TQString& );

protected:
    void keyPressEvent( TQKeyEvent * e);

private:
    void popupDetail( const TQString& contents, const TQPoint& pos );
    bool linksEnabled() const { return true; }
    void emitHighlighted( const TQString &s );
    void emitLinkClicked( const TQString &s );
    TQTextBrowserData *d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTextBrowser( const TQTextBrowser & );
    TQTextBrowser& operator=( const TQTextBrowser & );
#endif
};

#endif // TQT_NO_TEXTBROWSER

#endif // TQTEXTBROWSER_H
