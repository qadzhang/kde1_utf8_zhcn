/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQDIREXAMPLE_H
#define TQDIREXAMPLE_H

#include <ntqscrollview.h>
#include <ntqfiledialog.h>
#include <ntqwidgetstack.h>
#include <ntqvbox.h>
#include <ntqurl.h>
#include <ntqpixmap.h>
#include <ntqstringlist.h>

class TQMultiLineEdit;
class TQTextView;
class DirectoryView;
class TQSpinBox;
class TQShowEvent;
class TQPopupMenu;

class PixmapView : public TQScrollView
{
    TQ_OBJECT

public:
    PixmapView( TQWidget *parent );
    void setPixmap( const TQPixmap &pix );
    void drawContents( TQPainter *p, int, int, int, int );

private:
    TQPixmap pixmap;

};

class Preview : public TQWidgetStack
{
    TQ_OBJECT

public:
    Preview( TQWidget *parent );
    void showPreview( const TQUrl &u, int size );

private:
    TQMultiLineEdit *normalText;
    TQTextView *html;
    PixmapView *pixmap;

};

class PreviewWidget : public TQVBox,
		      public TQFilePreview
{
    TQ_OBJECT

public:
    PreviewWidget( TQWidget *parent );
    void previewUrl( const TQUrl &u );

private:
    TQSpinBox *sizeSpinBox;
    Preview *preview;

};

class CustomFileDialog : public TQFileDialog
{
    TQ_OBJECT

public:
    CustomFileDialog();
    ~CustomFileDialog();

protected:
    void showEvent( TQShowEvent *e );

public slots:
    void setDir2( const TQString & );

private slots:
    void bookmarkChosen( int i );
    void goHome();

private:
    DirectoryView *dirView;
    TQPopupMenu *bookmarkMenu;
    TQStringList bookmarkList;
    int addId;

};

#endif
