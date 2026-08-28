/****************************************************************************
**
** Definition of TQMessageBox class
**
** Created : 950503
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

#ifndef TQMESSAGEBOX_H
#define TQMESSAGEBOX_H

#ifndef QT_H
#include "ntqdialog.h"
#endif // QT_H

#ifndef TQT_NO_MESSAGEBOX

class  TQLabel;
class  TQPushButton;
struct TQMessageBoxData;

class TQ_EXPORT TQMessageBox : public TQDialog
{
    TQ_OBJECT
    TQ_ENUMS( Icon )
    TQ_PROPERTY( TQString text READ text WRITE setText )
    TQ_PROPERTY( Icon icon READ icon WRITE setIcon )
    TQ_PROPERTY( TQPixmap iconPixmap READ iconPixmap WRITE setIconPixmap )
    TQ_PROPERTY( TextFormat textFormat READ textFormat WRITE setTextFormat )

public:
    enum Icon { NoIcon = 0, Information = 1, Warning = 2, Critical = 3,
		Question = 4 };

    TQMessageBox( TQWidget* parent=0, const char* name=0 );
    TQMessageBox( const TQString& caption, const TQString &text, Icon icon,
		 int button0, int button1, int button2,
		 TQWidget* parent=0, const char* name=0, bool modal=true,
		 WFlags f=WStyle_DialogBorder  );
    ~TQMessageBox();

    enum { NoButton = 0, Ok = 1, Cancel = 2, Yes = 3, No = 4, Abort = 5,
	   Retry = 6, Ignore = 7, YesAll = 8, NoAll = 9, ButtonMask = 0xff,
	   Default = 0x100, Escape = 0x200, FlagMask = 0x300 };

    static int information( TQWidget *parent, const TQString &caption,
			    const TQString& text,
			    int button0, int button1=0, int button2=0 );
    static int information( TQWidget *parent, const TQString &caption,
			    const TQString& text,
			    const TQString& button0Text = TQString::null,
			    const TQString& button1Text = TQString::null,
			    const TQString& button2Text = TQString::null,
			    int defaultButtonNumber = 0,
			    int escapeButtonNumber = -1 );

    static int question( TQWidget *parent, const TQString &caption,
			 const TQString& text,
			 int button0, int button1=0, int button2=0 );
    static int question( TQWidget *parent, const TQString &caption,
			 const TQString& text,
			 const TQString& button0Text = TQString::null,
			 const TQString& button1Text = TQString::null,
			 const TQString& button2Text = TQString::null,
			 int defaultButtonNumber = 0,
			 int escapeButtonNumber = -1 );

    static int warning( TQWidget *parent, const TQString &caption,
			const TQString& text,
			int button0, int button1, int button2=0 );
    static int warning( TQWidget *parent, const TQString &caption,
			const TQString& text,
			const TQString& button0Text = TQString::null,
			const TQString& button1Text = TQString::null,
			const TQString& button2Text = TQString::null,
			int defaultButtonNumber = 0,
			int escapeButtonNumber = -1 );

    static int critical( TQWidget *parent, const TQString &caption,
			 const TQString& text,
			 int button0, int button1, int button2=0 );
    static int critical( TQWidget *parent, const TQString &caption,
			 const TQString& text,
			 const TQString& button0Text = TQString::null,
			 const TQString& button1Text = TQString::null,
			 const TQString& button2Text = TQString::null,
			 int defaultButtonNumber = 0,
			 int escapeButtonNumber = -1 );

    static void about( TQWidget *parent, const TQString &caption,
		       const TQString& text );

    static void aboutTQt( TQWidget *parent,
			 const TQString& caption=TQString::null );

/* OBSOLETE */
    static int message( const TQString &caption,
			const TQString& text,
			const TQString& buttonText=TQString::null,
			TQWidget *parent=0, const char * =0 ) {
	return TQMessageBox::information( parent, caption, text,
				     buttonText.isEmpty()
				     ? tr("OK") : buttonText ) == 0;
    }

/* OBSOLETE */
    static bool query( const TQString &caption,
		       const TQString& text,
		       const TQString& yesButtonText=TQString::null,
		       const TQString& noButtonText=TQString::null,
		       TQWidget *parent=0, const char * = 0 ) {
	return TQMessageBox::information( parent, caption, text,
				     yesButtonText.isEmpty()
				     ? tr("OK") : yesButtonText,
				     noButtonText ) == 0;
    }

    TQString	text() const;
    void	setText( const TQString &);

    Icon	icon() const;

    void	setIcon( Icon );
    void	setIcon( const TQPixmap & );

    const TQPixmap *iconPixmap() const;
    void	setIconPixmap( const TQPixmap & );

    TQString	buttonText( int button ) const;
    void	setButtonText( int button, const TQString &);

    void	adjustSize();

/* OBSOLETE */
    static TQPixmap standardIcon( Icon icon, GUIStyle );

    static TQPixmap standardIcon( Icon icon );

    TextFormat textFormat() const;
    void	 setTextFormat( TextFormat );

protected:
    void	resizeEvent( TQResizeEvent * );
    void	showEvent( TQShowEvent * );
    void	closeEvent( TQCloseEvent * );
    void	keyPressEvent( TQKeyEvent * );
    void	styleChanged( TQStyle& );

private slots:
    void	buttonClicked();

private:
    void	init( int, int, int );
    int		indexOf( int ) const;
    void	resizeButtons();
    TQLabel     *label;
    TQMessageBoxData *mbd;
    void       *reserved1;
    void       *reserved2;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQMessageBox( const TQMessageBox & );
    TQMessageBox &operator=( const TQMessageBox & );
#endif
};

/*
*  Macro to be used at the beginning of main(), e.g.
*
*   #include <ntqapplication.h>
*   #include <ntqmessagebox.h>
*   int main( int argc, char**argv )
*   {
*     QT_REQUIRE_VERSION( argc, argv, "3.0.5" )
*     ...
*   }
*/
#define QT_REQUIRE_VERSION( argc, argv, str ) { TQString s=TQString::fromLatin1(str);\
TQString sq=TQString::fromLatin1(tqVersion()); if ( (sq.section('.',0,0).toInt()<<16)+\
(sq.section('.',1,1).toInt()<<8)+sq.section('.',2,2).toInt()<(s.section('.',0,0).toInt()<<16)+\
(s.section('.',1,1).toInt()<<8)+s.section('.',2,2).toInt() ){if ( !tqApp){ int c=0; new \
TQApplication(argc,argv);} TQString s = TQApplication::tr("Executable '%1' requires TQt "\
 "%2, found TQt %3.").arg(TQString::fromLatin1(tqAppName())).arg(TQString::fromLatin1(\
str)).arg(TQString::fromLatin1(tqVersion()) ); TQMessageBox::critical( 0, TQApplication::tr(\
"Incompatible TQt Library Error" ), s, TQMessageBox::Abort,0 ); tqFatal(s.ascii()); }}


#endif // TQT_NO_MESSAGEBOX

#endif // TQMESSAGEBOX_H
