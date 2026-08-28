/****************************************************************************
**
** Definition of TQTabDialog class
**
** Created : 960825
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

#ifndef TQTABDIALOG_H
#define TQTABDIALOG_H

#ifndef QT_H
#include "ntqdialog.h"
#include "ntqiconset.h"
#endif // QT_H

#ifndef TQT_NO_TABDIALOG

class  TQTabBar;
class  TQTab;
class  TQTabDialogPrivate;

class TQ_EXPORT TQTabDialog : public TQDialog
{
    TQ_OBJECT
public:
    TQTabDialog( TQWidget* parent=0, const char* name=0, bool modal=false,
		WFlags f=0 );
    ~TQTabDialog();

    void show();
    void setFont( const TQFont & font );

    void addTab( TQWidget *, const TQString &);
    void addTab( TQWidget *child, const TQIconSet& iconset, const TQString &label);
    void addTab( TQWidget *, TQTab* );

    void insertTab( TQWidget *, const TQString &, int index = -1);
    void insertTab( TQWidget *child, const TQIconSet& iconset, const TQString &label, int index = -1);
    void insertTab( TQWidget *, TQTab*, int index = -1 );

    void changeTab( TQWidget *, const TQString &);
    void changeTab( TQWidget *child, const TQIconSet& iconset, const TQString &label);

    bool isTabEnabled(  TQWidget * ) const;
    void setTabEnabled( TQWidget *, bool );
    bool isTabEnabled( const char* ) const; // compatibility
    void setTabEnabled( const char*, bool ); // compatibility

    void showPage( TQWidget * );
    void removePage( TQWidget * );
    TQString tabLabel( TQWidget * );

    TQWidget * currentPage() const;

    void setDefaultButton( const TQString &text );
    void setDefaultButton();
    bool hasDefaultButton() const;

    void setHelpButton( const TQString &text );
    void setHelpButton();
    bool hasHelpButton() const;

    void setCancelButton( const TQString &text );
    void setCancelButton();
    bool hasCancelButton() const;

    void setApplyButton( const TQString &text );
    void setApplyButton();
    bool hasApplyButton() const;

#ifndef Q_QDOC
    void setOKButton( const TQString &text = TQString::null );
#endif
    void setOkButton( const TQString &text );
    void setOkButton();
    bool hasOkButton() const;

protected:
    void paintEvent( TQPaintEvent * );
    void resizeEvent( TQResizeEvent * );
    void styleChange( TQStyle& );
    void setTabBar( TQTabBar* );
    TQTabBar* tabBar() const;

signals:
    void aboutToShow();

    void applyButtonPressed();
    void cancelButtonPressed();
    void defaultButtonPressed();
    void helpButtonPressed();

    void currentChanged( TQWidget * );
    void selected( const TQString& ); // obsolete

private slots:
    void showTab( int i );

private:
    void setSizes();
    void setUpLayout();

    TQTabDialogPrivate *d;
private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQTabDialog( const TQTabDialog & );
    TQTabDialog& operator=( const TQTabDialog & );
#endif
};

#endif // TQT_NO_TABDIALOG

#endif // TQTABDIALOG_H
