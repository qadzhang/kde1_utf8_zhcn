/****************************************************************************
**
** Definition of TQTabWidget class
**
** Created : 990318
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

#ifndef TQTABWIDGET_H
#define TQTABWIDGET_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqiconset.h"
#endif // QT_H

#ifndef TQT_NO_TABWIDGET

class TQTabBar;
class TQTabWidgetData;
class TQTab;
class TQWidgetStack;


class TQ_EXPORT TQTabWidget : public TQWidget
{
    TQ_OBJECT
    TQ_ENUMS( TabPosition )
    TQ_ENUMS( TabShape )
    TQ_PROPERTY( TabPosition tabPosition READ tabPosition WRITE setTabPosition )
    TQ_PROPERTY( TabShape tabShape READ tabShape WRITE setTabShape )
    TQ_PROPERTY( int margin READ margin WRITE setMargin )
    TQ_PROPERTY( int currentPage READ currentPageIndex WRITE setCurrentPage )
    TQ_PROPERTY( int count READ count )
    TQ_OVERRIDE( bool autoMask DESIGNABLE true SCRIPTABLE true )

public:
    TQTabWidget( TQWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~TQTabWidget();

    virtual void addTab( TQWidget *, const TQString & ); // ### make these inline in 4.0
    virtual void addTab( TQWidget *child, const TQIconSet& iconset,
			 const TQString &label );
    virtual void addTab( TQWidget *, TQTab* );

    virtual void insertTab( TQWidget *, const TQString &, int index = -1 );
    virtual void insertTab( TQWidget *child, const TQIconSet& iconset,
			    const TQString &label, int index = -1 );
    virtual void insertTab( TQWidget *, TQTab*, int index = -1 );

    void changeTab( TQWidget *, const TQString &);
    void changeTab( TQWidget *child, const TQIconSet& iconset,
		    const TQString &label );

    bool isTabEnabled(  TQWidget * ) const;
    void setTabEnabled( TQWidget *, bool );

    void setCornerWidget( TQWidget * w, TQt::Corner corner = TQt::TopRight );
    TQWidget * cornerWidget( TQt::Corner corner = TQt::TopRight ) const;

    TQString tabLabel( TQWidget * ) const;
    void setTabLabel( TQWidget *p, const TQString &l );

    TQIconSet tabIconSet( TQWidget * w ) const;
    void setTabIconSet( TQWidget * w, const TQIconSet & iconset );

    void removeTabToolTip( TQWidget * w );
    void setTabToolTip( TQWidget * w, const TQString & tip );
    TQString tabToolTip( TQWidget * w ) const;

    TQWidget * currentPage() const;
    TQWidget *page( int ) const;
    TQString label( int ) const;
    int currentPageIndex() const;
    int indexOf( TQWidget * ) const;

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

    enum TabPosition { Top, Bottom };
    TabPosition tabPosition() const;
    void setTabPosition( TabPosition );

    enum TabShape { Rounded, Triangular };
    TabShape tabShape() const;
    void setTabShape( TabShape s );

    int margin() const;
    void setMargin( int );

    int count() const;

public slots:
    void setCurrentPage( int );
    virtual void showPage( TQWidget * );
    virtual void removePage( TQWidget * );

protected:
    void showEvent( TQShowEvent * );
    void resizeEvent( TQResizeEvent * );
    void setTabBar( TQTabBar * );
    TQTabBar* tabBar() const;
    void styleChange( TQStyle& );
    void updateMask();
    bool eventFilter( TQObject *, TQEvent * );

signals:
    void currentChanged( TQWidget * );
#ifndef Q_QDOC
    void selected( const TQString& );
#endif

private slots:
    void showTab( int );

private:
    TQTabWidgetData *d;
    void setUpLayout( bool = false );
    friend class TQTabDialog;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTabWidget( const TQTabWidget & );
    TQTabWidget& operator=( const TQTabWidget & );
#endif
};

#endif // TQT_NO_TABWIDGET

#endif // TQTABWIDGET_H
