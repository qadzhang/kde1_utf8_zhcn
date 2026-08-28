/****************************************************************************
** $Id: ntqinputcontext.h,v 1.8 2004/06/22 06:47:30 daisuke Exp $
**
** Definition of TQInputContext
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQINPUTCONTEXT_H
#define TQINPUTCONTEXT_H

#ifndef TQT_NO_IM

#ifndef QT_H
#include "ntqobject.h"
#include "ntqglobal.h"
#include "ntqevent.h"
#include "ntqstring.h"
#include "ntqptrlist.h"
#endif

class TQWidget;
class TQFont;
class TQPopupMenu;
class TQInputContextPrivate;


struct TQInputContextMenu {
    enum Action {
	NoSeparator,
	InsertSeparator
    };
#if !(TQT_VERSION-0 >= 0x040000)
    TQString title;
    TQPopupMenu *popup;
#endif
};


class TQ_EXPORT TQInputContext : public TQObject
{
    TQ_OBJECT
public:
    TQInputContext( TQObject *parent = 0 );
    virtual ~TQInputContext();

    virtual TQString identifierName();
    virtual TQString language();

#if defined(TQ_WS_X11)
    virtual bool x11FilterEvent( TQWidget *keywidget, XEvent *event );
#endif // TQ_WS_X11
    virtual bool filterEvent( const TQEvent *event );
    virtual void reset();

    virtual void setFocus();
    virtual void unsetFocus();
    virtual void setMicroFocus( int x, int y, int w, int h, TQFont *f = 0 );
    virtual void mouseHandler( int x, TQEvent::Type type,
			       TQt::ButtonState button, TQt::ButtonState state );
    virtual TQFont font() const;
    virtual bool isComposing() const;
    virtual bool isPreeditRelocationEnabled();

#if (TQT_VERSION-0 >= 0x040000)
    virtual TQPtrList<TQAction *> actions();
    void addActionsTo( TQMenu *menu, TQInputContextMenu::Action action = TQInputContextMenu::InsertSeparator );
#else
    virtual TQPtrList<TQInputContextMenu> *menus();
    void addMenusTo( TQPopupMenu *popup, TQInputContextMenu::Action action = TQInputContextMenu::InsertSeparator );
#endif

#if defined(TQ_WS_X11)
    // these functions are not recommended for ordinary use 
    virtual TQWidget *focusWidget() const;
    virtual TQWidget *holderWidget() const;

    // these functions must not be used by ordinary input method
    virtual void setFocusWidget( TQWidget *w );
    virtual void setHolderWidget( TQWidget *w );
    virtual void releaseComposingWidget( TQWidget *w );
#endif

signals:
    void deletionRequested();
    void imEventGenerated( TQObject *receiver, TQIMEvent *e );

protected:
    virtual void sendIMEvent( TQEvent::Type type,
			      const TQString &text = TQString::null,
			      int cursorPosition = -1, int selLength = 0 );

private:
    void sendIMEventInternal( TQEvent::Type type,
			      const TQString &text = TQString::null,
			      int cursorPosition = -1, int selLength = 0 );

    TQInputContextPrivate *d;

    friend class TQWidget;
    friend class TQInputContextFactory;

private:   // Disabled copy constructor and operator=
    TQInputContext( const TQInputContext & );
    TQInputContext &operator=( const TQInputContext & );

};

#endif //Q_NO_IM

#endif // TQINPUTCONTEXT_H
