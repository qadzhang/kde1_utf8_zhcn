/****************************************************************************
**
** Definition of TQTab and TQTabBar classes
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

#ifndef TQTABBAR_H
#define TQTABBAR_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqptrlist.h"
#endif // QT_H

#ifndef TQT_NO_TABBAR

class TQTabBar;
class TQIconSet;

class TQ_EXPORT TQTab : public TQt
{
    friend class TQTabBar;
    friend class TQTabWidget;

public:
    TQTab();
    virtual ~TQTab();
    TQTab( const TQString& text );
    TQTab( const TQIconSet& icon, const TQString& text = TQString::null );

    void setText( const TQString& text);
    TQString text() const { return label; }
    void setIconSet( const TQIconSet& icon );
    TQIconSet* iconSet() const { return iconset; }
    void setRect( const TQRect& rect ) { r = rect; }
    TQRect rect() const { return r; }
    void setEnabled( bool enable ) { enabled = enable; }
    bool isEnabled() const { return enabled; }
    void setIdentifier( int i ) { id = i; }
    int identifier() const { return id; }

private:
    void setTabBar( TQTabBar *tb );
    TQString label;
    TQRect r; // the bounding rectangle of this (may overlap with others)
    bool enabled;
    int id;
    TQIconSet* iconset; // optional iconset
    TQTabBar *tb;
};


struct TQTabPrivate;
//class *TQAccel;

class TQ_EXPORT TQTabBar: public TQWidget
{
    TQ_OBJECT
    TQ_ENUMS( Shape )
    TQ_PROPERTY( Shape shape READ shape WRITE setShape )
    TQ_PROPERTY( int currentTab READ currentTab WRITE setCurrentTab )
    TQ_PROPERTY( int count READ count )
    TQ_PROPERTY( int keyboardFocusTab READ keyboardFocusTab )

public:
    TQTabBar( TQWidget* parent=0, const char* name=0 );
    ~TQTabBar();

    enum Shape { RoundedAbove, RoundedBelow,
		 TriangularAbove, TriangularBelow };

    Shape shape() const;
    virtual void setShape( Shape );

    void show();

    virtual int addTab( TQTab * );
    virtual int insertTab( TQTab *, int index = -1 );
    virtual void removeTab( TQTab * );

    virtual void setTabEnabled( int, bool );
    bool isTabEnabled( int ) const;


    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    int currentTab() const;
    int keyboardFocusTab() const;

    TQTab * tab( int ) const;
    TQTab * tabAt( int ) const;
    int indexOf( int ) const;
    int count() const;

    virtual void layoutTabs();
    virtual TQTab * selectTab( const TQPoint & p ) const;

    void 	removeToolTip( int index );
    void     	setToolTip( int index, const TQString & tip );
    TQString 	toolTip( int index ) const;

public slots:
    virtual void setCurrentTab( int );
    virtual void setCurrentTab( TQTab * );

signals:
    void selected( int );
    void layoutChanged();

protected:
    virtual void paint( TQPainter *, TQTab *, bool ) const; // ### not const
    virtual void paintLabel( TQPainter*, const TQRect&, TQTab*, bool ) const;

    void focusInEvent( TQFocusEvent *e );
    void focusOutEvent( TQFocusEvent *e );

    void resizeEvent( TQResizeEvent * );
    void paintEvent( TQPaintEvent * );
    void mousePressEvent ( TQMouseEvent * );
    void mouseMoveEvent ( TQMouseEvent * );
    void mouseReleaseEvent ( TQMouseEvent * );
    void keyPressEvent( TQKeyEvent * );
    void styleChange( TQStyle& );
    void fontChange ( const TQFont & );

    bool event( TQEvent *e );
    
    TQPtrList<TQTab> * tabList();

private slots:
    void scrollTabs();

private:
    TQPtrList<TQTab> * l;
    TQPtrList<TQTab> * lstatic;
    void makeVisible( TQTab* t = 0 );
    void updateArrowButtons();
    TQTabPrivate * d;

    friend class TQTabBarToolTip;
    friend class TQTab;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTabBar( const TQTabBar & );
    TQTabBar& operator=( const TQTabBar & );
#endif

protected:
    void enterEvent ( TQEvent * );
    void leaveEvent ( TQEvent * );

private:
    TQTab *hoverTab;

public:
    TQTab *mouseHoverTab() const;
};


#endif // TQT_NO_TABBAR

#endif // TQTABBAR_H
