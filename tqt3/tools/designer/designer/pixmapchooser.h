/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef PIXMAPCHOOSER_H
#define PIXMAPCHOOSER_H

#include <ntqfiledialog.h>
#include <ntqscrollview.h>
#include <ntqpixmap.h>
#include <ntqurl.h>

class FormWindow;

class PixmapView : public TQScrollView,
		   public TQFilePreview
{
    TQ_OBJECT

public:
    PixmapView( TQWidget *parent );
    void setPixmap( const TQPixmap &pix );
    void drawContents( TQPainter *p, int, int, int, int );
    void previewUrl( const TQUrl &u );

private:
    TQPixmap pixmap;

};

class ImageIconProvider : public TQFileIconProvider
{
    TQ_OBJECT

public:
    ImageIconProvider( TQWidget *parent = 0, const char *name = 0 );
    ~ImageIconProvider();

    const TQPixmap *pixmap( const TQFileInfo &fi );

private:
    TQStrList fmts;
    TQPixmap imagepm;

};

TQPixmap qChoosePixmap( TQWidget *parent, FormWindow *fw = 0, const TQPixmap &old = TQPixmap(),  TQString *fn = 0 );
TQStringList qChoosePixmaps( TQWidget *parent );

#endif
