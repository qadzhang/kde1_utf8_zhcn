/****************************************************************************
**
** Definition of TQClipboard class
**
** Created : 960430
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQCLIPBOARD_H
#define TQCLIPBOARD_H

#ifndef QT_H
#include "ntqwindowdefs.h"
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_CLIPBOARD

class TQMimeSource;

class TQ_EXPORT TQClipboard : public TQObject
{
    TQ_OBJECT
private:
    TQClipboard( TQObject *parent=0, const char *name=0 );
    ~TQClipboard();

public:
    enum Mode { Clipboard, Selection };

    void	clear( Mode mode );	       	// ### default arg = Clipboard in 4.0
    void	clear();		       	// ### remove 4.0

    bool	supportsSelection() const;
    bool	ownsSelection() const;
    bool	ownsClipboard() const;

    void	setSelectionMode(bool enable);		// ### remove 4.0
    bool	selectionModeEnabled() const; 		// ### remove 4.0

    // ### default arg mode = Clipboard in 4.0 for all of these
    TQString     text( Mode mode )	 const;
    TQString     text( TQCString& subtype, Mode mode ) const;
    void	setText( const TQString &, Mode mode );

#ifndef TQT_NO_MIMECLIPBOARD
    TQMimeSource *data( Mode mode ) const;
    void setData( TQMimeSource*, Mode mode );

    TQImage	image( Mode mode ) const;
    TQPixmap	pixmap( Mode mode ) const;
    void	setImage( const TQImage &, Mode mode );
    void	setPixmap( const TQPixmap &, Mode mode );
#endif

    // ### remove all of these in 4.0
    TQString     text()	 const;
    TQString     text(TQCString& subtype) const;
    void	setText( const TQString &);

#ifndef TQT_NO_MIMECLIPBOARD
    TQMimeSource	*data() const;
    void	setData( TQMimeSource* );

    TQImage	image() const;
    TQPixmap	pixmap() const;
    void	setImage( const TQImage & );
    void	setPixmap( const TQPixmap & );
#endif

signals:
    void        selectionChanged();
    void	dataChanged();

private slots:
    void	ownerDestroyed();

protected:
    void	connectNotify( const char * );
    bool	event( TQEvent * );

    friend class TQApplication;
    friend class TQBaseApplication;
    friend class TQDragManager;
    friend class TQMimeSource;

private:
#if defined(TQ_WS_MAC)
    void loadScrap(bool convert);
    void saveScrap();
#endif

    // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQClipboard( const TQClipboard & );
    TQClipboard &operator=( const TQClipboard & );
#endif
};

#endif // TQT_NO_CLIPBOARD

#endif // TQCLIPBOARD_H
