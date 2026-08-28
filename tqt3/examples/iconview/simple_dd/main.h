/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqcursor.h>
#include <ntqsplitter.h>
#include <ntqlistbox.h>
#include <ntqiconview.h>
#include <ntqpixmap.h>

class TQDragEnterEvent;
class TQDragDropEvent;


class DDListBox : public TQListBox
{
    TQ_OBJECT
public:
    DDListBox( TQWidget * parent = 0, const char * name = 0, WFlags f = 0 );
    // Low-level drag and drop
    void dragEnterEvent( TQDragEnterEvent *evt );
    void dropEvent( TQDropEvent *evt );
    void mousePressEvent( TQMouseEvent *evt );
    void mouseMoveEvent( TQMouseEvent * );
private:
    int dragging;
};


class DDIconViewItem : public TQIconViewItem
{
public:
    DDIconViewItem( TQIconView *parent, const TQString& text, const TQPixmap& icon ) :
	TQIconViewItem( parent, text, icon ) {}
    DDIconViewItem( TQIconView *parent, const TQString &text ) :
	TQIconViewItem( parent, text ) {}
    // High-level drag and drop
    bool acceptDrop( const TQMimeSource *mime ) const;
    void dropped( TQDropEvent *evt, const TQValueList<TQIconDragItem>& );
};


class DDIconView : public TQIconView
{
    TQ_OBJECT
public:
    DDIconView( TQWidget * parent = 0, const char * name = 0, WFlags f = 0 ) :
	TQIconView( parent, name, f ) {}
    // High-level drag and drop
    TQDragObject *dragObject();
public slots:
    void slotNewItem( TQDropEvent *evt, const TQValueList<TQIconDragItem>& list );
};

