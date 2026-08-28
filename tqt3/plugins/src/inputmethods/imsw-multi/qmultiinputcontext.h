/****************************************************************************
** $Id$
**
** Definition of TQMultiInputContext class
**
** Copyright (C) 2004 immodule for TQt Project.  All rights reserved.
**
** This file is written to contribute to Trolltech AS under their own
** licence. You may use this file under your TQt license. Following
** description is copied from their original file headers. Contact
** immodule-qt@freedesktop.org if any conditions of this licensing are
** not clear to you.
**
**
** This file is part of the input method module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef TQMULTIINPUTCONTEXT_H
#define TQMULTIINPUTCONTEXT_H

#ifndef TQT_NO_IM

#include <ntqnamespace.h>
#include <ntqwidget.h>
#include <ntqinputcontext.h>
#include <ntqguardedptr.h>
#include <ntqintdict.h>

class TQPopupMenu;

class TQMultiInputContext : public TQInputContext
{
    TQ_OBJECT
public:
    TQMultiInputContext();
    ~TQMultiInputContext();

    TQString identifierName();
    TQString language();

#if defined(TQ_WS_X11)
    bool x11FilterEvent( TQWidget *keywidget, XEvent *event );
#endif // TQ_WS_X11
    bool filterEvent( const TQEvent *event );
    void reset();

    void setFocus();
    void unsetFocus();
    void setMicroFocus( int x, int y, int w, int h, TQFont *f = 0 );
    void mouseHandler( int x, TQEvent::Type type,
		       TQt::ButtonState button, TQt::ButtonState state );
    TQFont font() const;
    bool isComposing() const;
    bool isPreeditRelocationEnabled();
    
#if (TQT_VERSION-0 >= 0x040000)
    TQPtrList<TQMenu> *qt4menus();
#endif
    TQPtrList<TQInputContextMenu> *menus();
    TQPopupMenu *createImSelPopup();    

#if defined(TQ_WS_X11)
    TQWidget *focusWidget() const;
    TQWidget *holderWidget() const;

    void setFocusWidget( TQWidget *w );
    void setHolderWidget( TQWidget *w );
    void releaseComposingWidget( TQWidget *w );
#endif

public slots:
    virtual void destroyInputContext();
    virtual void postIMEvent( TQObject *receiver, TQIMEvent *event );

protected slots:
    void changeInputMethodWithMenuId( int menuid );

protected:
    TQInputContext *slave();
    const TQInputContext *slave() const;
    
    void changeInputMethod( int newIndex );
    void changeInputMethod( TQString name );

    TQInputContext *_slave;
    int imIndex;
    bool cachedFocus;
    TQWidget *cachedFocusWidget;
    TQWidget *cachedHolderWidget;
    bool beIndirectlyConnected;

    TQIntDict<TQString> keyDict;
    TQGuardedPtr<TQPopupMenu> popup;
    TQString currentIMKey;
};

#endif //Q_NO_IM

#endif // TQMULTIINPUTCONTEXT_H
