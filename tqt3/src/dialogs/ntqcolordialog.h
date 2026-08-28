/****************************************************************************
**
** Definition of TQColorDialog class
**
** Created : 990222
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

#ifndef TQCOLORDIALOG_H
#define TQCOLORDIALOG_H

#ifndef QT_H
#include "ntqdialog.h"
#endif // QT_H

#ifndef TQT_NO_COLORDIALOG

class TQColorDialogPrivate;

class TQ_EXPORT TQColorDialog : public TQDialog
{
    TQ_OBJECT

public:
    static TQColor getColor( const TQColor& init = white, TQWidget* parent=0, const char* name=0 );
    static TQRgb getRgba( TQRgb, bool* ok = 0,
			 TQWidget* parent=0, const char* name=0 );

    static int customCount();
    static TQRgb customColor( int );
    static void setCustomColor( int, TQRgb );
    static void setStandardColor( int, TQRgb );

private:
    ~TQColorDialog();
    TQColorDialog( TQWidget* parent=0, const char* name=0, bool modal=false );

    void setColor( const TQColor& );
    TQColor color() const;

    bool selectColor( const TQColor& );

    void setSelectedAlpha( int );
    int selectedAlpha() const;

    void showCustom( bool=true );

private:	// Disabled copy constructor and operator=
    TQColorDialogPrivate *d;
    friend class TQColorDialogPrivate;
    friend class TQColorShower;

#if defined(TQ_DISABLE_COPY)
    TQColorDialog( const TQColorDialog & );
    TQColorDialog& operator=( const TQColorDialog & );
#endif
};

#endif

#endif //TQCOLORDIALOG_H
