/****************************************************************************
**
** Definition of TQProgressDialog class
**
** Created : 970520
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

#ifndef TQPROGRESSDIALOG_H
#define TQPROGRESSDIALOG_H

#ifndef QT_H
#include "ntqsemimodal.h"
#include "ntqlabel.h"       // ### remove or keep for users' convenience?
#include "ntqprogressbar.h" // ### remove or keep for users' convenience?
#endif // QT_H

#ifndef TQT_NO_PROGRESSDIALOG

class TQPushButton;
class TQTimer;
class TQProgressDialogData;

class TQ_EXPORT TQProgressDialog : public TQDialog
{
    TQ_OBJECT
    TQ_PROPERTY( bool wasCancelled READ wasCancelled DESIGNABLE false STORED false ) // ### remove in 4.0
    TQ_PROPERTY( bool wasCanceled READ wasCanceled )
    TQ_PROPERTY( int totalSteps READ totalSteps WRITE setTotalSteps )
    TQ_PROPERTY( int progress READ progress WRITE setProgress )
    TQ_PROPERTY( bool autoReset READ autoReset WRITE setAutoReset )
    TQ_PROPERTY( bool autoClose READ autoClose WRITE setAutoClose )
    TQ_PROPERTY( int minimumDuration READ minimumDuration WRITE setMinimumDuration )
    TQ_PROPERTY( TQString labelText READ labelText WRITE setLabelText )

public:
    TQProgressDialog( TQWidget* parent=0, const char* name=0, bool modal=false,
		     WFlags f=0 );
    TQProgressDialog( const TQString& labelText, const TQString &cancelButtonText,
		     int totalSteps, TQWidget* parent=0, const char* name=0,
		     bool modal=false, WFlags f=0 );
    ~TQProgressDialog();

    void	setLabel( TQLabel * );
    void	setCancelButton( TQPushButton * );
    void	setBar( TQProgressBar * );

    // ### TQt 4.0: remove wasCancelled() in 4.0
    bool	wasCancelled() const;
    inline bool	wasCanceled() const { return wasCancelled(); }

    int		totalSteps() const;
    int		progress()   const;

    TQSize	sizeHint() const;

    TQString     labelText() const;

    void setAutoReset( bool b );
    bool autoReset() const;
    void setAutoClose( bool b );
    bool autoClose() const;

public slots:
    void	cancel();
    void	reset();
    void	setTotalSteps( int totalSteps );
    void	setProgress( int progress );
    void	setProgress( int progress, int totalSteps );
    void	setLabelText( const TQString &);
    void	setCancelButtonText( const TQString &);

    void	setMinimumDuration( int ms );
public:
    int		minimumDuration() const;

signals:
    // ### remove cancelled() in 4.0
    void	cancelled();
    void	canceled();

protected:
    void	resizeEvent( TQResizeEvent * );
    void	closeEvent( TQCloseEvent * );
    void	styleChange( TQStyle& );
    void	showEvent( TQShowEvent *e );

protected slots:
    void	forceShow();

private:
    void	   init( TQWidget *creator, const TQString& lbl, const TQString &canc,
			 int totstps);
    void	   layout();
    TQLabel	  *label()  const;
    TQProgressBar  *bar()    const;
    TQProgressDialogData *d;
    TQTimer	  *forceTimer;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQProgressDialog( const TQProgressDialog & );
    TQProgressDialog &operator=( const TQProgressDialog & );
#endif
};

#endif // TQT_NO_PROGRESSDIALOG

#endif // TQPROGRESSDIALOG_H
