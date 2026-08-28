/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <ntqtextbrowser.h>

class MainWindow;
class TQKeyEvent;
class TQMime;
class TQMouseEvent;

class HelpWindow : public TQTextBrowser
{
    TQ_OBJECT
public:
    HelpWindow( MainWindow *m, TQWidget *parent = 0, const char *name = 0 );
    void setSource( const TQString &name );
    TQPopupMenu *createPopupMenu( const TQPoint& pos );
    void blockScrolling( bool b );
    void openLinkInNewWindow( const TQString &link );
    void openLinkInNewPage( const TQString &link );
    void addMimePath( const TQString &path );

    void contentsMousePressEvent(TQMouseEvent *e);
    void keyPressEvent(TQKeyEvent *);

    bool isBackwardAvailable() const { return backAvail; }
    bool isForwardAvailable() const { return fwdAvail; }

signals:
    void chooseWebBrowser();

public slots:
    void copy();
protected slots:
    void ensureCursorVisible();

private slots:
    void openLinkInNewWindow();
    void openLinkInNewPage();
    void updateForward(bool);
    void updateBackward(bool);

private:
    MainWindow *mw;
    TQString lastAnchor;
    bool blockScroll;
    bool shiftPressed;
    bool newWindow;
    TQMimeSourceFactory *mimeSourceFactory;
    bool fwdAvail;
    bool backAvail;
};

#endif
