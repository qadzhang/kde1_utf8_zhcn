/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DIRVIEW_H
#define DIRVIEW_H

#include <ntqlistview.h>
#include <ntqstring.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqtimer.h>

class TQWidget;
class TQDragEnterEvent;
class TQDragMoveEvent;
class TQDragLeaveEvent;
class TQDropEvent;

class FileItem : public TQListViewItem
{
public:
    FileItem( TQListViewItem *parent, const TQString &s1, const TQString &s2 )
	: TQListViewItem( parent, s1, s2 ), pix( 0 ) {}

    const TQPixmap *pixmap( int i ) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQListViewItem::setPixmap;
#endif
    void setPixmap( TQPixmap *p );

private:
    TQPixmap *pix;

};

class Directory : public TQListViewItem
{
public:
    Directory( TQListView * parent, const TQString& filename );
    Directory( Directory * parent, const TQString& filename, const TQString &col2 )
	: TQListViewItem( parent, filename, col2 ), pix( 0 ) {}
    Directory( Directory * parent, const TQString& filename );

    TQString text( int column ) const;

    TQString fullName();

    void setOpen( bool );
    void setup();

    const TQPixmap *pixmap( int i ) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQListViewItem::setPixmap;
#endif
    void setPixmap( TQPixmap *p );

private:
    TQFile f;
    Directory * p;
    bool readable;
    bool showDirsOnly;
    TQPixmap *pix;

};

class DirectoryView : public TQListView
{
    TQ_OBJECT

public:
    DirectoryView( TQWidget *parent = 0, const char *name = 0, bool sdo = false );
    bool showDirsOnly() { return dirsOnly; }

public slots:
    void setDir( const TQString & );

signals:
    void folderSelected( const TQString & );

protected slots:
    void slotFolderSelected( TQListViewItem * );
    void openFolder();

protected:
    void contentsDragEnterEvent( TQDragEnterEvent *e );
    void contentsDragMoveEvent( TQDragMoveEvent *e );
    void contentsDragLeaveEvent( TQDragLeaveEvent *e );
    void contentsDropEvent( TQDropEvent *e );
    void contentsMouseMoveEvent( TQMouseEvent *e );
    void contentsMousePressEvent( TQMouseEvent *e );
    void contentsMouseReleaseEvent( TQMouseEvent *e );

private:
    TQString fullPath(TQListViewItem* item);
    bool dirsOnly;
    TQListViewItem *oldCurrent;
    TQListViewItem *dropItem;
    TQTimer* autoopen_timer;
    TQPoint presspos;
    bool mousePressed;

};

#endif
