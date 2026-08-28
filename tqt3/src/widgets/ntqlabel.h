/**********************************************************************
**
** Definition of TQLabel widget class
**
** Created : 941215
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

#ifndef TQLABEL_H
#define TQLABEL_H

#ifndef QT_H
#include "ntqframe.h"
#endif // QT_H

#ifndef TQT_NO_LABEL

class TQSimpleRichText;
class TQLabelPrivate;

class TQ_EXPORT TQLabel : public TQFrame
{
    TQ_OBJECT
    TQ_PROPERTY( TQString text READ text WRITE setText )
    TQ_PROPERTY( TextFormat textFormat READ textFormat WRITE setTextFormat )
    TQ_PROPERTY( TQPixmap pixmap READ pixmap WRITE setPixmap )
    TQ_PROPERTY( bool scaledContents READ hasScaledContents WRITE setScaledContents )
    TQ_PROPERTY( Alignment alignment READ alignment WRITE setAlignment )
    TQ_PROPERTY( int indent READ indent WRITE setIndent )
    TQ_OVERRIDE( BackgroundMode backgroundMode DESIGNABLE true)

public:
    TQLabel( TQWidget *parent, const char* name=0, WFlags f=0 );
    TQLabel( const TQString &text, TQWidget *parent, const char* name=0,
	    WFlags f=0 );
    TQLabel( TQWidget *buddy, const TQString &,
	    TQWidget *parent, const char* name=0, WFlags f=0 );
    ~TQLabel();

    TQString	 text()		const	{ return ltext; }
    TQPixmap     *pixmap()	const	{ return lpixmap; }
#ifndef TQT_NO_PICTURE
    TQPicture    *picture()	const	{ return lpicture; }
#endif
#ifndef TQT_NO_MOVIE
    TQMovie      *movie()		const;
#endif

    TextFormat textFormat() const;
    void 	 setTextFormat( TextFormat );

    int		 alignment() const	{ return align; }
    virtual void setAlignment( int );
    int		 indent() const		{ return extraMargin; }
    void 	 setIndent( int );

    bool 	 autoResize() const	{ return autoresize; }
    virtual void setAutoResize( bool );
#ifndef TQT_NO_IMAGE_SMOOTHSCALE
    bool 	hasScaledContents() const;
    void 	setScaledContents( bool );
#endif
    TQSize	 sizeHint() const;
    TQSize	 minimumSizeHint() const;
#ifndef TQT_NO_ACCEL
    virtual void setBuddy( TQWidget * );
    TQWidget     *buddy() const;
#endif
    int		 heightForWidth(int) const;

    void setFont( const TQFont &f );

public slots:
    virtual void setText( const TQString &);
    virtual void setPixmap( const TQPixmap & );
#ifndef TQT_NO_PICTURE
    virtual void setPicture( const TQPicture & );
#endif
#ifndef TQT_NO_MOVIE
    virtual void setMovie( const TQMovie & );
#endif
    virtual void setNum( int );
    virtual void setNum( double );
    void	 clear();

protected:
    void	 drawContents( TQPainter * );
    void	 fontChange( const TQFont & );
    void	 resizeEvent( TQResizeEvent* );

private slots:
#ifndef TQT_NO_ACCEL
    void	 acceleratorSlot();
    void	 buddyDied();
#endif
#ifndef TQT_NO_MOVIE
    void	 movieUpdated(const TQRect&);
    void	 movieResized(const TQSize&);
#endif

private:
    void	init();
    void	clearContents();
    void	updateLabel( TQSize oldSizeHint );
    TQSize	sizeForWidth( int w ) const;
    TQString	ltext;
    TQPixmap    *lpixmap;
#ifndef TQT_NO_PICTURE
    TQPicture   *lpicture;
#endif
#ifndef TQT_NO_MOVIE
    TQMovie *	lmovie;
#endif
#ifndef TQT_NO_ACCEL
    TQWidget *	lbuddy;
#endif
    ushort	align;
    short	extraMargin;
    uint	autoresize:1;
    uint	scaledcontents :1;
    TextFormat textformat;
#ifndef TQT_NO_RICHTEXT
    TQSimpleRichText* doc;
#endif
#ifndef TQT_NO_ACCEL
    TQAccel *	accel;
#endif
    TQLabelPrivate* d;

    friend class TQTipLabel;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQLabel( const TQLabel & );
    TQLabel &operator=( const TQLabel & );
#endif
};


#endif // TQT_NO_LABEL

#endif // TQLABEL_H
