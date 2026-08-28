/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef IMAGETEXTEDITOR_H
#define IMAGETEXTEDITOR_H

#include <ntqdialog.h>

class TQImage;
class TQComboBox;
class TQListBox;
class TQLineEdit;
class TQMultiLineEdit;

class ImageTextEditor : public TQDialog
{
    TQ_OBJECT
public:
    ImageTextEditor( TQImage& i, TQWidget *parent=0, const char *name=0, WFlags f=0 );
    ~ImageTextEditor();
    void accept();
public slots:
    void imageChanged();
    void updateText();
    void addText();
    void removeText();
private:
    void storeText();
    TQImage& image;
    TQComboBox* languages;
    TQComboBox* keys;
    TQMultiLineEdit* text;
    TQLineEdit* newlang;
    TQLineEdit* newkey;
    TQString currKey();
    TQString currLang();
    TQString currText();
};

#endif // IMAGETEXTEDITOR_H
