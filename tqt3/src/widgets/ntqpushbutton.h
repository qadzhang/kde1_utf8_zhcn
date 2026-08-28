/****************************************************************************
**
** Definition of TQPushButton class
**
** Created : 940221
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

#ifndef TQPUSHBUTTON_H
#define TQPUSHBUTTON_H

#ifndef QT_H
#include "ntqbutton.h"
#include "ntqiconset.h"
#endif // QT_H

#ifndef TQT_NO_PUSHBUTTON
class TQPushButtonPrivate;
class TQPopupMenu;

class TQ_EXPORT TQPushButton : public TQButton
{
    TQ_OBJECT

    TQ_PROPERTY( bool autoDefault READ autoDefault WRITE setAutoDefault )
    TQ_PROPERTY( bool default READ isDefault WRITE setDefault )
    TQ_PROPERTY( bool menuButton READ isMenuButton DESIGNABLE false )
    TQ_PROPERTY( TQIconSet iconSet READ iconSet WRITE setIconSet )
    TQ_OVERRIDE( bool toggleButton WRITE setToggleButton )
    TQ_OVERRIDE( bool on WRITE setOn )
    TQ_PROPERTY( bool flat READ isFlat WRITE setFlat )
    TQ_OVERRIDE( bool autoMask DESIGNABLE true SCRIPTABLE true )

public:
    TQPushButton( TQWidget *parent, const char* name=0 );
    TQPushButton( const TQString &text, TQWidget *parent, const char* name=0 );
#ifndef TQT_NO_ICONSET
    TQPushButton( const TQIconSet& icon, const TQString &text, TQWidget *parent, const char* name=0 );
#endif
    ~TQPushButton();

    TQSize	sizeHint() const;

    void	move( int x, int y );
    void	move( const TQPoint &p );
    void	resize( int w, int h );
    void	resize( const TQSize & );
    void	setGeometry( int x, int y, int w, int h );

    void	setGeometry( const TQRect & );

    void setToggleButton( bool );

    bool	autoDefault()	const	{ return autoDefButton; }
    virtual void setAutoDefault( bool autoDef );
    bool	isDefault()	const	{ return defButton; }
    virtual void setDefault( bool def );

    virtual void setIsMenuButton( bool enable ) {  // obsolete functions
	if ( (bool)hasMenuArrow == enable )
	    return;
	hasMenuArrow = enable ? 1 : 0;
	update();
	updateGeometry();
    }
    bool	isMenuButton() const { return hasMenuArrow; }

#ifndef TQT_NO_POPUPMENU
    void setPopup( TQPopupMenu* popup );
    TQPopupMenu* popup() const;
#endif
#ifndef TQT_NO_ICONSET
    void setIconSet( const TQIconSet& );
    TQIconSet* iconSet() const;
#endif
    void setFlat( bool );
    bool isFlat() const;

public slots:
    virtual void setOn( bool );

protected:
    void	drawButton( TQPainter * );
    void	drawButtonLabel( TQPainter * );
    void	focusInEvent( TQFocusEvent * );
    void	focusOutEvent( TQFocusEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	updateMask();
private slots:
#ifndef TQT_NO_POPUPMENU
    void popupPressed();
#endif
private:
    void	init();

    uint	autoDefButton	: 1;
    uint	defButton	: 1;
    uint	flt		: 1;
    uint	reserved		: 1; // UNUSED
    uint	lastEnabled	: 1; // UNUSED
    uint	hasMenuArrow	: 1;

    TQPushButtonPrivate* d;

    friend class TQDialog;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQPushButton( const TQPushButton & );
    TQPushButton &operator=( const TQPushButton & );
#endif
};


#endif // TQT_NO_PUSHBUTTON

#endif // TQPUSHBUTTON_H
