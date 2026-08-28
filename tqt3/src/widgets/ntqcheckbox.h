/****************************************************************************
**
** Definition of TQCheckBox class
**
** Created : 940222
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

#ifndef TQCHECKBOX_H
#define TQCHECKBOX_H

#ifndef QT_H
#include "ntqbutton.h"
#endif // QT_H

#ifndef TQT_NO_CHECKBOX

class TQ_EXPORT TQCheckBox : public TQButton
{
    TQ_OBJECT
    TQ_PROPERTY( bool checked READ isChecked WRITE setChecked )
    TQ_PROPERTY( bool tristate READ isTristate WRITE setTristate )
    TQ_OVERRIDE( bool autoMask DESIGNABLE true SCRIPTABLE true )

public:
    TQCheckBox( TQWidget *parent, const char* name=0 );
    TQCheckBox( const TQString &text, TQWidget *parent, const char* name=0 );

    bool    isChecked() const;

    void    setNoChange();

    void    setTristate(bool y=true);
    bool    isTristate() const;

    TQSize   sizeHint() const;

public slots:
    void    setChecked( bool check );

protected:
    void    resizeEvent( TQResizeEvent* );
    void    drawButton( TQPainter * );
    void    drawButtonLabel( TQPainter * );
    void    updateMask();
    bool    hitButton( const TQPoint &pos ) const;
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQCheckBox( const TQCheckBox & );
    TQCheckBox &operator=( const TQCheckBox & );
#endif
};


inline bool TQCheckBox::isChecked() const
{ return isOn(); }

inline void TQCheckBox::setChecked( bool check )
{ setOn( check ); }


#endif // TQT_NO_CHECKBOX

#endif // TQCHECKBOX_H
