/****************************************************************************
**
** Definition of TQMenuBar class
**
** Created : 941209
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

#ifndef TQMENUBAR_H
#define TQMENUBAR_H

#ifndef QT_H
#include "ntqpopupmenu.h" // ### remove or keep for users' convenience?
#include "ntqframe.h"
#include "ntqmenudata.h"
#endif // QT_H

#ifndef TQT_NO_MENUBAR

class TQPopupMenu;

class TQ_EXPORT TQMenuBar : public TQFrame, public TQMenuData
{
    TQ_OBJECT
    TQ_ENUMS( Separator )
    TQ_PROPERTY( Separator separator READ separator WRITE setSeparator DESIGNABLE false )
    TQ_PROPERTY( bool defaultUp READ isDefaultUp WRITE setDefaultUp )

public:
    TQMenuBar( TQWidget* parent=0, const char* name=0 );
    ~TQMenuBar();

    void	updateItem( int id );

    void	show();				// reimplemented show
    void	hide();				// reimplemented hide

    bool	eventFilter( TQObject *, TQEvent * );

    int		heightForWidth(int) const;

    enum	Separator { Never=0, InWindowsStyle=1 };
    Separator 	separator() const;
    virtual void	setSeparator( Separator when );

    void	setDefaultUp( bool );
    bool	isDefaultUp() const;

    bool customWhatsThis() const;

    TQSize sizeHint() const;
    TQSize minimumSize() const;
    TQSize minimumSizeHint() const;

    void activateItemAt( int index );

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    static void initialize();
    static void cleanup();
#endif

signals:
    void	activated( int itemId );
    void	highlighted( int itemId );

protected:
    void	drawContents( TQPainter * );
    void	fontChange( const TQFont & );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
    void	keyPressEvent( TQKeyEvent * );
    void	focusInEvent( TQFocusEvent * );
    void	focusOutEvent( TQFocusEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	leaveEvent( TQEvent * );
    void	menuContentsChanged();
    void	menuStateChanged();
    void 	styleChange( TQStyle& );
    int	itemAtPos( const TQPoint & );
    void	hidePopups();
    TQRect	itemRect( int item );

private slots:
    void	subActivated( int itemId );
    void	subHighlighted( int itemId );
#ifndef TQT_NO_ACCEL
    void	accelActivated( int itemId );
    void	accelDestroyed();
#endif
    void	popupDestroyed( TQObject* );
    void 	performDelayedChanges();

    void	languageChange();

private:
    void 	performDelayedContentsChanged();
    void 	performDelayedStateChanged();
    void	menuInsPopup( TQPopupMenu * );
    void	menuDelPopup( TQPopupMenu * );
    void	frameChanged();

    bool	tryMouseEvent( TQPopupMenu *, TQMouseEvent * );
    void	tryKeyEvent( TQPopupMenu *, TQKeyEvent * );
    void	goodbye( bool cancelled = false );
    void	openActPopup();

    void setActiveItem( int index, bool show = true, bool activate_first_item = true );
    void setAltMode( bool );

    int		calculateRects( int max_width = -1 );

#ifndef TQT_NO_ACCEL
    void	setupAccelerators();
    TQAccel     *autoaccel;
#endif
    TQRect      *irects;
    int		rightSide;

    uint	mseparator : 1;
    uint	waitforalt : 1;
    uint	popupvisible  : 1;
    uint	hasmouse : 1;
    uint 	defaultup : 1;
    uint 	toggleclose : 1;
    uint        pendingDelayedContentsChanges : 1;
    uint        pendingDelayedStateChanges : 1;

    friend class TQPopupMenu;

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    friend class TQWidget;
    friend class TQApplication;
    friend void tqt_mac_set_modal_state(bool, TQMenuBar *);

    void macCreateNativeMenubar();
    void macRemoveNativeMenubar();
    void macDirtyNativeMenubar();

#if !defined(TQMAC_QMENUBAR_NO_EVENT)
    static void tqt_mac_install_menubar_event(MenuRef);
    static OSStatus tqt_mac_menubar_event(EventHandlerCallRef, EventRef, void *);
#endif
    virtual void macWidgetChangedWindow();
    bool syncPopups(MenuRef ret, TQPopupMenu *d);
    MenuRef createMacPopup(TQPopupMenu *d, int id, bool =false);
    bool updateMenuBar();
#if !defined(TQMAC_QMENUBAR_NO_MERGE)
    uint isCommand(TQMenuItem *, bool just_check=false);
#endif

    uint mac_eaten_menubar : 1;
    class MacPrivate;
    MacPrivate *mac_d;
    static bool activate(MenuRef, short, bool highlight=false, bool by_accel=false);
    static bool activateCommand(uint cmd);
    static bool macUpdateMenuBar();
    static bool macUpdatePopupVisible(MenuRef, bool);
    static bool macUpdatePopup(MenuRef);
#endif

private:	// Disabled copy constructor and operator=

#if defined(TQ_DISABLE_COPY)
    TQMenuBar( const TQMenuBar & );
    TQMenuBar &operator=( const TQMenuBar & );
#endif
};


#endif // TQT_NO_MENUBAR

#endif // TQMENUBAR_H
