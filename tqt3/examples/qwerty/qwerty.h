/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TQWERTY_H
#define TQWERTY_H

#include <ntqwidget.h>
#include <ntqmenubar.h>
#include <ntqmultilineedit.h>
#include <ntqprinter.h>

class Editor : public TQWidget
{
    TQ_OBJECT
public:
    Editor( TQWidget *parent=0, const char *name="qwerty" );
   ~Editor();

    void load( const TQString& fileName, int code=-1 );

public slots:
    void newDoc();
    void load();
    bool save();
    void print();
    void addEncoding();
    void toUpper();
    void toLower();
    void font();
protected:
    void resizeEvent( TQResizeEvent * );
    void closeEvent( TQCloseEvent * );

private slots:
    void saveAsEncoding( int );
    void openAsEncoding( int );
    void textChanged();

private:
    bool saveAs( const TQString& fileName, int code=-1 );
    void rebuildCodecList();
    TQMenuBar 	   *m;
    TQMultiLineEdit *e;
#ifndef TQT_NO_PRINTER
    TQPrinter        printer;
#endif
    TQPopupMenu	   *save_as;
    TQPopupMenu	   *open_as;
    bool changed;
};

#endif // TQWERTY_H
