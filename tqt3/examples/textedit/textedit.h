/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <ntqmainwindow.h>
#include <ntqmap.h>

class TQAction;
class TQComboBox;
class TQTabWidget;
class TQTextEdit;

class TextEdit : public TQMainWindow
{
    TQ_OBJECT

public:
    TextEdit( TQWidget *parent = 0, const char *name = 0 );

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();
    void load( const TQString &f );
    TQTextEdit *currentEditor() const;
    void doConnections( TQTextEdit *e );

private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void fileClose();
    void fileExit();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily( const TQString &f );
    void textSize( const TQString &p );
    void textColor();
    void textAlign( TQAction *a );

    void fontChanged( const TQFont &f );
    void colorChanged( const TQColor &c );
    void alignmentChanged( int a );
    void editorChanged( TQWidget * );

private:
    TQAction *actionTextBold,
	*actionTextUnderline,
	*actionTextItalic,
	*actionTextColor,
	*actionAlignLeft,
	*actionAlignCenter,
	*actionAlignRight,
	*actionAlignJustify;
    TQComboBox
	*comboFont,
	*comboSize;
    TQTabWidget *tabWidget;
    TQMap<TQTextEdit*, TQString> filenames;

};


#endif
