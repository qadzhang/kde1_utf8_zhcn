/****************************************************************************
**
** Definition of TQFontDialog
**
** Created : 970605
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the dialogs module of the TQt GUI Toolkit.
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

#ifndef TQFONTDIALOG_H
#define TQFONTDIALOG_H

#include "ntqwindowdefs.h"

#ifndef TQT_NO_FONTDIALOG

//
//  W A R N I N G
//  -------------
//
//  This class is under development and has private constructors.
//
//  You may use the public static getFont() functions which are guaranteed
//  to be available in the future.
//

#ifndef QT_H
#include "ntqdialog.h"
#include "ntqfont.h"
#endif // QT_H

class TQFontDialogPrivate;

class TQ_EXPORT TQFontDialog: public TQDialog
{
    TQ_OBJECT

public:
    static TQFont getFont( bool *ok, const TQFont &def,
			  TQWidget* parent=0, const char* name=0);
    static TQFont getFont( bool *ok, TQWidget* parent=0, const char* name=0);

private:
    static TQFont getFont( bool *ok, const TQFont *def,
			  TQWidget* parent=0, const char* name=0);

    TQFontDialog( TQWidget* parent=0, const char* name=0, bool modal=false,
		 WFlags f=0 );
    ~TQFontDialog();

    TQFont font() const;
    void setFont( const TQFont &font );

    bool eventFilter( TQObject *, TQEvent * );

    void updateFamilies();
    void updateStyles();
    void updateSizes();

private slots:
    void sizeChanged( const TQString &);
    void familyHighlighted( int );
    void scriptHighlighted( int );
    void styleHighlighted( int );
    void sizeHighlighted( const TQString & );
    void updateSample();

private:
    friend class TQFontDialogPrivate;
    TQFontDialogPrivate * d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQFontDialog( const TQFontDialog & );
    TQFontDialog& operator=( const TQFontDialog & );
#endif
};

#endif

#endif // TQFONTDIALOG_H
