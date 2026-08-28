/**********************************************************************
**
** Definition of TQGridView class
**
** Created : 010523
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

#ifndef TQGRIDVIEW_H
#define TQGRIDVIEW_H

#ifndef QT_H
#include "ntqscrollview.h"
#endif // QT_H

#ifndef TQT_NO_GRIDVIEW

class TQGridViewPrivate;

class TQ_EXPORT TQGridView : public TQScrollView
{
    TQ_OBJECT
    TQ_PROPERTY( int numRows READ numRows WRITE setNumRows )
    TQ_PROPERTY( int numCols READ numCols WRITE setNumCols )
    TQ_PROPERTY( int cellWidth READ cellWidth WRITE setCellWidth )
    TQ_PROPERTY( int cellHeight READ cellHeight WRITE setCellHeight )
public:

    TQGridView( TQWidget *parent=0, const char *name=0, WFlags f=0 );
   ~TQGridView();

    int numRows() const;
    virtual void setNumRows( int );
    int numCols() const;
    virtual void setNumCols( int );

    int cellWidth() const;
    virtual void setCellWidth( int );
    int cellHeight() const;
    virtual void setCellHeight( int );
    
    TQRect cellRect() const;
    TQRect cellGeometry( int row, int column );
    TQSize gridSize() const;

    int rowAt( int y ) const;
    int columnAt( int x ) const;

    void repaintCell( int row, int column, bool erase=true );
    void updateCell( int row, int column );
    void ensureCellVisible( int row, int column );

protected:
    virtual void paintCell( TQPainter *, int row, int col ) = 0;
    virtual void paintEmptyArea( TQPainter *p, int cx, int cy, int cw, int ch );

    void drawContents( TQPainter *p, int cx, int cy, int cw, int ch );

    virtual void dimensionChange( int, int );

private:
    void drawContents( TQPainter* );
    void updateGrid();

    int nrows;
    int ncols;
    int cellw;
    int cellh;
    TQGridViewPrivate* d;

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQGridView( const TQGridView & );
    TQGridView &operator=( const TQGridView & );
#endif
};

inline int TQGridView::cellWidth() const 
{ return cellw; }

inline int TQGridView::cellHeight() const 
{ return cellh; }

inline int TQGridView::rowAt( int y ) const 
{ return y / cellh; }

inline int TQGridView::columnAt( int x ) const 
{ return x / cellw; }

inline int TQGridView::numRows() const 
{ return nrows; }

inline int TQGridView::numCols() const 
{return ncols; }

inline TQRect TQGridView::cellRect() const
{ return TQRect( 0, 0, cellw, cellh ); }

inline TQSize TQGridView::gridSize() const 
{ return TQSize( ncols * cellw, nrows * cellh ); }



#endif // TQT_NO_GRIDVIEW


#endif // TQTABLEVIEW_H
