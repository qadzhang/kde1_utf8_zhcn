/****************************************************************************
**
** Definition of TQDialog class
**
** Created : 950502
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

#ifndef TQDIALOG_H
#define TQDIALOG_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H
#ifndef TQT_NO_DIALOG
#if 0
TQ_OBJECT
#endif

class TQPushButton;
class TQDialogPrivate;

class TQ_EXPORT TQDialog : public TQWidget
{
friend class TQPushButton;
    TQ_OBJECT
    TQ_PROPERTY( bool sizeGripEnabled READ isSizeGripEnabled WRITE setSizeGripEnabled )
    TQ_PROPERTY( bool modal READ isModal WRITE setModal )

public:
    explicit TQDialog( TQWidget* parent=0, const char* name=0, bool modal=false,
	     WFlags f=0 );
    ~TQDialog();

    enum DialogCode { Rejected, Accepted };

    int		result() const { return rescode; }

    void	show();
    void	hide();
    void	move( int x, int y );
    void	move( const TQPoint &p );
    void	resize( int w, int h );
    void	resize( const TQSize & );
    void	setGeometry( int x, int y, int w, int h );
    void	setGeometry( const TQRect & );

    void	setOrientation( Orientation orientation );
    Orientation	orientation() const;

    void	setExtension( TQWidget* extension );
    TQWidget*	extension() const;

    TQSize	sizeHint() const;
    TQSize	minimumSizeHint() const;

    void setSizeGripEnabled( bool );
    bool isSizeGripEnabled() const;

    void setModal( bool modal );
    bool isModal() const;
#ifdef Q_OS_TEMP
    bool	event( TQEvent * );
#endif

public slots:
    int exec();

protected slots:
    virtual void done( int );
    virtual void accept();
    virtual void reject();

    void	showExtension( bool );

protected:
    void	setResult( int r )	{ rescode = r; }
    void	keyPressEvent( TQKeyEvent * );
    void	closeEvent( TQCloseEvent * );
    void	resizeEvent( TQResizeEvent * );
    void	contextMenuEvent( TQContextMenuEvent * );
    bool	eventFilter( TQObject *, TQEvent * );
    void	adjustPosition( TQWidget*);

private:
    void	setDefault( TQPushButton * );
    void	setMainDefault( TQPushButton * );
    void	hideDefault();
#ifdef Q_OS_TEMP
    void	hideSpecial();
#endif

    int		rescode;
    uint	did_move   : 1;
    uint	has_relpos : 1;
    uint	did_resize : 1;
    uint	in_loop: 1;
    void adjustPositionInternal( TQWidget*, bool useRelPos = false );
    TQDialogPrivate* d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQDialog( const TQDialog & );
    TQDialog &operator=( const TQDialog & );
#endif
};

#endif // TQT_NO_DIALOG
#endif // TQDIALOG_H
