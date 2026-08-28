/****************************************************************************
**
** Definition of TQToolBox widget class
**
** Created : 961105
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

#ifndef TQTOOLBOX_H
#define TQTOOLBOX_H

#ifndef QT_H
#include <ntqframe.h>
#include <ntqiconset.h>
#endif // QT_H

#ifndef TQT_NO_TOOLBOX

class TQToolBoxPrivate;
class TQWidgetList;

class TQ_EXPORT TQToolBox : public TQFrame
{
    TQ_OBJECT
    TQ_PROPERTY( int currentIndex READ currentIndex WRITE setCurrentIndex )
    TQ_PROPERTY( int count READ count )

public:
    TQToolBox( TQWidget *parent = 0, const char *name = 0, WFlags f = 0 );
    ~TQToolBox();

    int addItem( TQWidget *item, const TQString &label );
    int addItem( TQWidget *item, const TQIconSet &iconSet, const TQString &label );
    int insertItem( int index, TQWidget *item, const TQString &label );
    int insertItem( int index, TQWidget *item, const TQIconSet &iconSet, const TQString &label );

    int removeItem( TQWidget *item );

    void setItemEnabled( int index, bool enabled );
    bool isItemEnabled( int index ) const;

    void setItemLabel( int index, const TQString &label );
    TQString itemLabel( int index ) const;

    void setItemIconSet( int index, const TQIconSet &iconSet );
    TQIconSet itemIconSet( int index ) const;

    void setItemToolTip( int index, const TQString &toolTip );
    TQString itemToolTip( int index ) const;

    TQWidget *currentItem() const;
    void setCurrentItem( TQWidget *item );

    int currentIndex() const;
    TQWidget *item( int index ) const;
    int indexOf( TQWidget *item ) const;
    int count() const;

public slots:
    void setCurrentIndex( int index );

signals:
    void currentChanged( int index );

private slots:
    void buttonClicked();
    void itemDestroyed(TQObject*);

protected:
    virtual void itemInserted( int index );
    virtual void itemRemoved( int index );
    void showEvent( TQShowEvent *e );
    void frameChanged();
    void styleChange(TQStyle&);

private:
    void relayout();

private:
    TQToolBoxPrivate *d;

};


inline int TQToolBox::addItem( TQWidget *item, const TQString &label )
{ return insertItem( -1, item, TQIconSet(), label ); }
inline int TQToolBox::addItem( TQWidget *item, const TQIconSet &iconSet,
			      const TQString &label )
{ return insertItem( -1, item, iconSet, label ); }
inline int TQToolBox::insertItem( int index, TQWidget *item, const TQString &label )
{ return insertItem( index, item, TQIconSet(), label ); }

#endif // TQT_NO_TOOLBOX
#endif
