/****************************************************************************
**
** Definition of TQFrame widget class
**
** Created : 950201
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

#ifndef TQFRAME_H
#define TQFRAME_H

#ifndef QT_H
#include "ntqwidget.h"
#endif // QT_H

#ifndef TQT_NO_FRAME

class TQ_EXPORT TQFrame : public TQWidget
{
    TQ_OBJECT
    TQ_ENUMS( Shape Shadow )
    TQ_PROPERTY( int frameWidth READ frameWidth )
    TQ_PROPERTY( TQRect contentsRect READ contentsRect )
    TQ_PROPERTY( Shape frameShape READ frameShape WRITE setFrameShape )
    TQ_PROPERTY( Shadow frameShadow READ frameShadow WRITE setFrameShadow )
    TQ_PROPERTY( int lineWidth READ lineWidth WRITE setLineWidth )
    TQ_PROPERTY( int margin READ margin WRITE setMargin )
    TQ_PROPERTY( int midLineWidth READ midLineWidth WRITE setMidLineWidth )
    TQ_PROPERTY( TQRect frameRect READ frameRect WRITE setFrameRect DESIGNABLE false )

public:
    TQFrame( TQWidget* parent=0, const char* name=0, WFlags f=0 );

    int         frameStyle()    const;
    virtual void setFrameStyle( int );

    int         frameWidth()    const;
    TQRect       contentsRect()  const;

#ifndef Q_QDOC
    bool        lineShapesOk()  const { return true; }
#endif

    TQSize       sizeHint() const;

    enum Shape { NoFrame  = 0,                  // no frame
                 Box      = 0x0001,             // rectangular box
                 Panel    = 0x0002,             // rectangular panel
                 WinPanel = 0x0003,             // rectangular panel (Windows)
                 HLine    = 0x0004,             // horizontal line
                 VLine    = 0x0005,             // vertical line
                 StyledPanel = 0x0006,          // rectangular panel depending on the GUI style
                 PopupPanel = 0x0007,           // rectangular panel depending on the GUI style
                 MenuBarPanel = 0x0008,
                 ToolBarPanel = 0x0009,
		 LineEditPanel = 0x000a,
		 TabWidgetPanel = 0x000b,
		 GroupBoxPanel = 0x000c,
                 MShape   = 0x000f              // mask for the shape
    };
    enum Shadow { Plain    = 0x0010,            // plain line
                  Raised   = 0x0020,            // raised shadow effect
                  Sunken   = 0x0030,            // sunken shadow effect
                  MShadow  = 0x00f0 };          // mask for the shadow

    Shape       frameShape()    const;
    void        setFrameShape( Shape );
    Shadow      frameShadow()   const;
    void        setFrameShadow( Shadow );

    int         lineWidth()     const;
    virtual void setLineWidth( int );

    int         margin()        const;
    virtual void setMargin( int );

    int         midLineWidth()  const;
    virtual void setMidLineWidth( int );

    TQRect       frameRect()     const;
    virtual void setFrameRect( const TQRect & );

protected:
    void        paintEvent( TQPaintEvent * );
    void        resizeEvent( TQResizeEvent * );
    virtual void drawFrame( TQPainter * );
    virtual void drawContents( TQPainter * );
    virtual void frameChanged();
    void        styleChange( TQStyle& );

private:
    void        updateFrameWidth(bool=false);
    TQRect       frect;
    int         fstyle;
    short       lwidth;
    short       mwidth;
    short       mlwidth;
    short       fwidth;

    void * d;
private:        // Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQFrame( const TQFrame & );
    TQFrame &operator=( const TQFrame & );
#endif
};


inline int TQFrame::frameStyle() const
{ return fstyle; }

inline TQFrame::Shape TQFrame::frameShape() const
{ return (Shape) ( fstyle & MShape ); }

inline TQFrame::Shadow TQFrame::frameShadow() const
{ return (Shadow) ( fstyle & MShadow ); }

inline void TQFrame::setFrameShape( TQFrame::Shape s )
{ setFrameStyle( ( fstyle & MShadow ) | s ); }

inline void TQFrame::setFrameShadow( TQFrame::Shadow s )
{ setFrameStyle( ( fstyle & MShape ) | s ); }

inline int TQFrame::lineWidth() const
{ return lwidth; }

inline int TQFrame::midLineWidth() const
{ return mlwidth; }

inline int TQFrame::margin() const
{ return mwidth; }

inline int TQFrame::frameWidth() const
{ return fwidth; }


#endif // TQT_NO_FRAME

#endif // TQFRAME_H
