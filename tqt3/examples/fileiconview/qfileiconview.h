/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQTFILEICONVIEW_H
#define TQTFILEICONVIEW_H


#include <ntqiconset.h>
#include <ntqstring.h>
#include <ntqfileinfo.h>
#include <ntqdir.h>
#include <ntqtimer.h>
#include <ntqiconview.h>

class TQtFileIconView;
class TQDragObject;
class TQResizeEvent;

/*****************************************************************************
 *
 * Class TQtFileIconDrag
 *
 *****************************************************************************/

class TQtFileIconDrag : public TQIconDrag
{
    TQ_OBJECT

public:
    TQtFileIconDrag( TQWidget * dragSource, const char* name = 0 );

    const char* format( int i ) const;
    TQByteArray encodedData( const char* mime ) const;
    static bool canDecode( TQMimeSource* e );
    void append( const TQIconDragItem &item, const TQRect &pr, const TQRect &tr, const TQString &url );

private:
    TQStringList urls;

};

/*****************************************************************************
 *
 * Class TQtFileIconView
 *
 *****************************************************************************/
class TQtFileIconViewItem;
class TQtFileIconView : public TQIconView
{
    TQ_OBJECT

public:
    TQtFileIconView( const TQString &dir, TQWidget *parent = 0, const char *name = 0 );

    enum ViewMode { Large, Small };

    void setViewMode( ViewMode m );
    ViewMode viewMode() const { return vm; }
    void setOpenItem( TQtFileIconViewItem *i ) {
	openItem = i;
    }

public slots:
    void setDirectory( const TQString &dir );
    void setDirectory( const TQDir &dir );
    void newDirectory();
    TQDir currentDir();

signals:
    void directoryChanged( const TQString & );
    void startReadDir( int dirs );
    void readNextDir();
    void readDirDone();
    void enableUp();
    void disableUp();
    void enableMkdir();
    void disableMkdir();

protected slots:
    void itemDoubleClicked( TQIconViewItem *i );
    void slotDropped( TQDropEvent *e, const TQValueList<TQIconDragItem> & );

    void viewLarge();
    void viewSmall();
    void viewBottom();
    void viewRight();
    void flowEast();
    void flowSouth();
    void itemTextTruncate();
    void itemTextWordWrap();
    void sortAscending();
    void sortDescending();
    void arrangeItemsInGrid() {
	TQIconView::arrangeItemsInGrid( true );
    }

    void slotRightPressed( TQIconViewItem *item );
    void openFolder();

protected:
    void readDir( const TQDir &dir );
    virtual TQDragObject *dragObject();

    virtual void keyPressEvent( TQKeyEvent *e );

    TQDir viewDir;
    int newFolderNum;
    TQSize sz;
    TQPixmap pix;
    ViewMode vm;
    TQtFileIconViewItem *openItem;

};

/*****************************************************************************
 *
 * Class TQtFileIconViewItem
 *
 *****************************************************************************/

class TQtFileIconViewItem : public TQIconViewItem
{
    friend class TQtFileIconView;

public:
    enum ItemType {
        File = 0,
        Dir,
        Link
    };

    TQtFileIconViewItem( TQtFileIconView *parent, TQFileInfo *fi );

    virtual ~TQtFileIconViewItem();

    ItemType type() const
    { return itemType; }
    TQString filename() const { return itemFileName; }

    virtual bool acceptDrop( const TQMimeSource *e ) const;

    virtual void setText( const TQString &text );
    virtual TQPixmap *pixmap() const;

    virtual void dragEntered();
    virtual void dragLeft();

    void viewModeChanged( TQtFileIconView::ViewMode m );
    void paintItem( TQPainter *p, const TQColorGroup &cg );

protected:
    virtual void dropped( TQDropEvent *e, const TQValueList<TQIconDragItem> & );

    TQString itemFileName;
    TQFileInfo *itemFileInfo;
    ItemType itemType;
    bool checkSetText;
    TQTimer timer;
    TQtFileIconView::ViewMode vm;

};


#endif
