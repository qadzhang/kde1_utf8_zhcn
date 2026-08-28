/****************************************************************************
**
** Definition of TQAction class
**
** Created : 000000
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQACTION_H
#define TQACTION_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqiconset.h"
#include "ntqstring.h"
#include "ntqkeysequence.h"
#endif // QT_H

#ifndef TQT_NO_ACTION

class TQActionPrivate;
class TQActionGroupPrivate;
class TQStatusBar;
class TQPopupMenu;
class TQToolTipGroup;

class TQ_EXPORT TQAction : public TQObject
{
    TQ_OBJECT
    TQ_PROPERTY( bool toggleAction READ isToggleAction WRITE setToggleAction)
    TQ_PROPERTY( bool on READ isOn WRITE setOn )
    TQ_PROPERTY( bool enabled READ isEnabled WRITE setEnabled )
    TQ_PROPERTY( TQIconSet iconSet READ iconSet WRITE setIconSet )
    TQ_PROPERTY( TQString text READ text WRITE setText )
    TQ_PROPERTY( TQString menuText READ menuText WRITE setMenuText )
    TQ_PROPERTY( TQString toolTip READ toolTip WRITE setToolTip )
    TQ_PROPERTY( TQString statusTip READ statusTip WRITE setStatusTip )
    TQ_PROPERTY( TQString whatsThis READ whatsThis WRITE setWhatsThis )
#ifndef TQT_NO_ACCEL
    TQ_PROPERTY( TQKeySequence accel READ accel WRITE setAccel )
#endif
    TQ_PROPERTY( bool visible READ isVisible WRITE setVisible )

public:
    TQAction( TQObject* parent, const char* name = 0 );
#ifndef TQT_NO_ACCEL
    TQAction( const TQString& menuText, TQKeySequence accel,
	     TQObject* parent, const char* name = 0 );
    TQAction( const TQIconSet& icon, const TQString& menuText, TQKeySequence accel,
	     TQObject* parent, const char* name = 0 );

    TQAction( const TQString& text, const TQIconSet& icon, const TQString& menuText, TQKeySequence accel,
	     TQObject* parent, const char* name = 0, bool toggle = false ); // obsolete
    TQAction( const TQString& text, const TQString& menuText, TQKeySequence accel, TQObject* parent,
	     const char* name = 0, bool toggle = false ); // obsolete
#endif
    TQAction( TQObject* parent, const char* name , bool toggle ); // obsolete
    ~TQAction();

    virtual void setIconSet( const TQIconSet& );
    TQIconSet iconSet() const;
    virtual void setText( const TQString& );
    TQString text() const;
    virtual void setMenuText( const TQString& );
    TQString menuText() const;
    virtual void setToolTip( const TQString& );
    TQString toolTip() const;
    virtual void setStatusTip( const TQString& );
    TQString statusTip() const;
    virtual void setWhatsThis( const TQString& );
    TQString whatsThis() const;
#ifndef TQT_NO_ACCEL
    virtual void setAccel( const TQKeySequence& key );
    TQKeySequence accel() const;
#endif
    virtual void setToggleAction( bool );

    bool isToggleAction() const;
    bool isOn() const;
    bool isEnabled() const;
    bool isVisible() const;
    virtual bool addTo( TQWidget* );
    virtual bool removeFrom( TQWidget* );

protected:
    virtual void addedTo( TQWidget *actionWidget, TQWidget *container );
    virtual void addedTo( int index, TQPopupMenu *menu );

public slots:
    void activate();
    void toggle();
    virtual void setOn( bool );
    virtual void setEnabled( bool );
    void setDisabled( bool );
    void setVisible( bool );

signals:
    void activated();
    void toggled( bool );

private slots:
    void internalActivation();
    void toolButtonToggled( bool );
    void objectDestroyed();
    void menuStatusText( int id );
    void showStatusText( const TQString& );
    void clearStatusText();

private:
    void init();

    friend class TQActionGroup;
    friend class TQActionGroupPrivate;
    TQActionPrivate* d;

#if defined(TQ_DISABLE_COPY)  // Disabled copy constructor and operator=
    TQAction( const TQAction & );
    TQAction &operator=( const TQAction & );
#endif
};

class TQ_EXPORT TQActionGroup : public TQAction
{
    TQ_OBJECT
    TQ_PROPERTY( bool exclusive READ isExclusive WRITE setExclusive )
    TQ_PROPERTY( bool usesDropDown READ usesDropDown WRITE setUsesDropDown )

public:
    TQActionGroup( TQObject* parent, const char* name = 0 );
    TQActionGroup( TQObject* parent, const char* name , bool exclusive  ); // obsolete
    ~TQActionGroup();
    void setExclusive( bool );
    bool isExclusive() const;
    void add( TQAction* a);
    void addSeparator();
    bool addTo( TQWidget* );
    bool removeFrom( TQWidget* );
    void setEnabled( bool );
    void setToggleAction( bool toggle );
    void setOn( bool on );

    void setUsesDropDown( bool enable );
    bool usesDropDown() const;

    void setIconSet( const TQIconSet& );
    void setText( const TQString& );
    void setMenuText( const TQString& );
    void setToolTip( const TQString& );
    void setWhatsThis( const TQString& );

protected:
    void childEvent( TQChildEvent* );
    virtual void addedTo( TQWidget *actionWidget, TQWidget *container, TQAction *a );
    virtual void addedTo( int index, TQPopupMenu *menu, TQAction *a );
    virtual void addedTo( TQWidget *actionWidget, TQWidget *container );
    virtual void addedTo( int index, TQPopupMenu *menu );

signals:
    void selected( TQAction* );

private slots:
    void childToggled( bool );
    void childDestroyed();
    void internalComboBoxActivated( int );
    void internalComboBoxHighlighted( int );
    void internalToggle( TQAction* );
    void objectDestroyed();

private:
    TQActionGroupPrivate* d;

#ifndef TQT_NO_COMPAT
public:
    void insert( TQAction* a ) { add( a ); }
#endif

private:
#if defined(TQ_DISABLE_COPY)  // Disabled copy constructor and operator=
    TQActionGroup( const TQActionGroup & );
    TQActionGroup &operator=( const TQActionGroup & );
#endif
};

#endif

#endif
