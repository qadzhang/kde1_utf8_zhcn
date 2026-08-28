/****************************************************************************
**
** Definition of TQButtonGroup class
**
** Created : 950130
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

#ifndef TQBUTTONGROUP_H
#define TQBUTTONGROUP_H

#ifndef QT_H
#include "ntqgroupbox.h"
#endif // QT_H

#ifndef TQT_NO_BUTTONGROUP


class TQButton;
class TQButtonList;


class TQ_EXPORT TQButtonGroup : public TQGroupBox
{
    TQ_OBJECT
    TQ_PROPERTY( bool exclusive READ isExclusive WRITE setExclusive )
    TQ_PROPERTY( bool radioButtonExclusive READ isRadioButtonExclusive WRITE setRadioButtonExclusive )
    TQ_PROPERTY( int selectedId READ selectedId WRITE setButton )

public:
    TQButtonGroup( TQWidget* parent=0, const char* name=0 );
    TQButtonGroup( const TQString &title,
		  TQWidget* parent=0, const char* name=0 );
    TQButtonGroup( int columns, Orientation o,
		  TQWidget* parent=0, const char* name=0 );
    TQButtonGroup( int columns, Orientation o, const TQString &title,
		  TQWidget* parent=0, const char* name=0 );
    ~TQButtonGroup();

    bool	isExclusive() const;
    bool	isRadioButtonExclusive() const { return radio_excl; }
    virtual void setExclusive( bool );
    virtual void setRadioButtonExclusive( bool );

public:
    int		insert( TQButton *, int id=-1 );
    void	remove( TQButton * );
    TQButton    *find( int id ) const;
    int		id( TQButton * ) const;
    int		count() const;

    virtual void setButton( int id );

    virtual void moveFocus( int );

    TQButton    *selected() const;
    int    selectedId() const;

signals:
    void	pressed( int id );
    void	released( int id );
    void	clicked( int id );

protected slots:
    void	buttonPressed();
    void	buttonReleased();
    void	buttonClicked();
    void	buttonToggled( bool on );

protected:
    bool 	event( TQEvent * e );

private:
    void	init();
    bool	excl_grp;
    bool	radio_excl;
    TQButtonList *buttons;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQButtonGroup( const TQButtonGroup & );
    TQButtonGroup &operator=( const TQButtonGroup & );
#endif
};


#endif // TQT_NO_BUTTONGROUP

#endif // TQBUTTONGROUP_H
