/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#ifndef STYLEDBUTTON_H
#define STYLEDBUTTON_H

#include <ntqbutton.h>
#include <ntqpixmap.h>

class TQColor;
class TQBrush;

class StyledButton : public TQButton
{
    TQ_OBJECT

    TQ_PROPERTY( TQColor color READ color WRITE setColor )
    TQ_PROPERTY( TQPixmap pixmap READ pixmap WRITE setPixmap )
    TQ_PROPERTY( EditorType editor READ editor WRITE setEditor )
    TQ_PROPERTY( bool scale READ scale WRITE setScale )

    TQ_ENUMS( EditorType )

public:
    enum EditorType { ColorEditor, PixmapEditor };

    StyledButton( TQWidget* parent = 0, const char* name = 0 );
    StyledButton( const TQBrush& b, TQWidget* parent = 0, const char* name = 0, WFlags f = 0 );
    ~StyledButton();

    void setEditor( EditorType );
    EditorType editor() const;

    void setColor( const TQColor& );
    void setPixmap( const TQPixmap& );

    TQPixmap* pixmap() const;
    TQColor color() const;

    void setScale( bool );
    bool scale() const;

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void onEditor();

signals:
    void changed();

protected:
    void mousePressEvent(TQMouseEvent*);
    void mouseMoveEvent(TQMouseEvent*);
#ifndef TQT_NO_DRAGANDDROP
    void dragEnterEvent ( TQDragEnterEvent * );
    void dragMoveEvent ( TQDragMoveEvent * );
    void dragLeaveEvent ( TQDragLeaveEvent * );
    void dropEvent ( TQDropEvent * );
#endif // TQT_NO_DRAGANDDROP
    void drawButton( TQPainter* );
    void drawButtonLabel( TQPainter* );
    void resizeEvent( TQResizeEvent* );
    void scalePixmap();

private:
    TQPixmap* pix;
    TQPixmap* spix;  // the pixmap scaled down to fit into the button
    TQColor col;
    EditorType edit;
    bool s;
    TQPoint pressPos;
    bool mousePressed;
};

#endif //STYLEDBUTTON_H
