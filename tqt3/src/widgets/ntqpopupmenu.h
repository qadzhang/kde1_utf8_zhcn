/****************************************************************************
**
** Definition of TQPopupMenu class
**
** Created : 941128
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

#ifndef TQPOPUPMENU_H
#define TQPOPUPMENU_H

#ifndef QT_H
#include "ntqframe.h"
#include "ntqmenudata.h"
#endif // QT_H

#ifndef TQT_NO_POPUPMENU
class TQPopupMenuPrivate;

class TQ_EXPORT TQPopupMenu : public TQFrame, public TQMenuData
{
    TQ_OBJECT
    TQ_PROPERTY( bool checkable READ isCheckable WRITE setCheckable )
public:
    TQPopupMenu( TQWidget* parent=0, const char* name=0 );
    ~TQPopupMenu();

    void	popup( const TQPoint & pos, int indexAtPoint = -1 ); // open
    void	updateItem( int id );

    virtual void	setCheckable( bool );
    bool	isCheckable() const;

    void	setFont( const TQFont & );
    void	show();
    void	hide();

    int		exec();
    int 	exec( const TQPoint & pos, int indexAtPoint = 0 ); // modal

    virtual void	setActiveItem( int );
    TQSize	sizeHint() const;

    int 	idAt( int index ) const { return TQMenuData::idAt( index ); }
    int 	idAt( const TQPoint& pos ) const;

    bool 	customWhatsThis() const;

    int		insertTearOffHandle( int id=-1, int index=-1 );

    void	activateItemAt( int index );
    TQRect	itemGeometry( int index );


signals:
    void	activated( int itemId );
    void	highlighted( int itemId );
    void	activatedRedirect( int itemId ); // to parent menu
    void	highlightedRedirect( int itemId );
    void	aboutToShow();
    void	aboutToHide();

protected:
    int 	itemHeight( int ) const;
    int 	itemHeight( TQMenuItem* mi ) const;
    void 	drawItem( TQPainter* p, int tab, TQMenuItem* mi,
		   bool act, int x, int y, int w, int h);

    void 	drawContents( TQPainter * );

    void 	closeEvent( TQCloseEvent *e );
    void	paintEvent( TQPaintEvent * );
    void	mousePressEvent( TQMouseEvent * );
    void	mouseReleaseEvent( TQMouseEvent * );
    void	mouseMoveEvent( TQMouseEvent * );
    void	keyPressEvent( TQKeyEvent * );
    void	focusInEvent( TQFocusEvent * );
    void	focusOutEvent( TQFocusEvent * );
    void	timerEvent( TQTimerEvent * );
    void	leaveEvent( TQEvent * );
    void 	styleChange( TQStyle& );
    void	enabledChange( bool );
    int 	columns() const;

    bool	focusNextPrevChild( bool next );

    int		itemAtPos( const TQPoint &, bool ignoreSeparator = true ) const;

private slots:
    void	subActivated( int itemId );
    void	subHighlighted( int itemId );
#ifndef TQT_NO_ACCEL
    void	accelActivated( int itemId );
    void	accelDestroyed();
#endif
    void	popupDestroyed( TQObject* );
    void	modalActivation( int );

    void	subMenuTimer();
    void	subScrollTimer();
    void	allowAnimation();
    void     toggleTearOff();

    void        performDelayedChanges();

private:
    void        updateScrollerState();
    void	menuContentsChanged();
    void	menuStateChanged();
    void        performDelayedContentsChanged();
    void        performDelayedStateChanged();
    void	menuInsPopup( TQPopupMenu * );
    void	menuDelPopup( TQPopupMenu * );
    void	frameChanged();

    void	actSig( int, bool = false );
    void	hilitSig( int );
    virtual void setFirstItemActive();
    void	hideAllPopups();
    void	hidePopups();
    bool	tryMenuBar( TQMouseEvent * );
    void	byeMenuBar();

    TQSize	updateSize(bool force_recalc=false, bool do_resize=true);
    void	updateRow( int row );
    TQRect       screenRect(const TQPoint& pos);
#ifndef TQT_NO_ACCEL
    void	updateAccel( TQWidget * );
    void	enableAccel( bool );
#endif
    TQPopupMenuPrivate  *d;
#ifndef TQT_NO_ACCEL
    TQAccel     *autoaccel;
#endif

#if defined(TQ_WS_MAC) && !defined(TQMAC_QMENUBAR_NO_NATIVE)
    bool macPopupMenu(const TQPoint &, int);
    uint mac_dirty_popup : 1;
#endif

    int popupActive;
    int tab;
    uint accelDisabled : 1;
    uint checkable : 1;
    uint connectModalRecursionSafety : 1;
    uint tornOff : 1;
    uint pendingDelayedContentsChanges : 1;
    uint pendingDelayedStateChanges : 1;
    int maxPMWidth;
    int ncols;
    bool	snapToMouse;
    bool	tryMouseEvent( TQPopupMenu *, TQMouseEvent * );

    friend class TQMenuData;
    friend class TQMenuBar;

    void connectModal(TQPopupMenu* receiver, bool doConnect);

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQPopupMenu( const TQPopupMenu & );
    TQPopupMenu &operator=( const TQPopupMenu & );
#endif

public:
    static int menuItemHeight( TQMenuItem* mi, TQFontMetrics fm );
};


#endif // TQT_NO_POPUPMENU

#endif // TQPOPUPMENU_H
