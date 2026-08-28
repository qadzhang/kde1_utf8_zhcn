/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "imagetexteditor.h"
#include <ntqimage.h>
#include <ntqlayout.h>
#include <ntqgrid.h>
#include <ntqvbox.h>
#include <ntqhbox.h>
#include <ntqcombobox.h>
#include <ntqmultilineedit.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqlistbox.h>
#include <ntqpushbutton.h>


ImageTextEditor::ImageTextEditor( TQImage& i, TQWidget *parent, const char *name, WFlags f ) :
    TQDialog(parent,name,true,f),
    image(i)
{
    TQVBoxLayout* vbox = new TQVBoxLayout(this,8);
    vbox->setAutoAdd(true);

    TQGrid* controls = new TQGrid(3,TQGrid::Horizontal,this);
    controls->setSpacing(8);
    TQLabel* l;
    l=new TQLabel("Language",controls); l->setAlignment(AlignCenter);
    l=new TQLabel("Key",controls); l->setAlignment(AlignCenter);
    (void)new TQLabel("",controls); // dummy
    languages = new TQComboBox(controls);
    keys = new TQComboBox(controls);
    TQPushButton* remove = new TQPushButton("Remove",controls);

    newlang = new TQLineEdit(controls);
    newkey = new TQLineEdit(controls);
    TQPushButton* add = new TQPushButton("Add",controls);

    text = new TQMultiLineEdit(this);

    TQHBox* hbox = new TQHBox(this);
    TQPushButton* cancel = new TQPushButton("Cancel",hbox);
    TQPushButton* ok = new TQPushButton("OK",hbox);

    connect(add,TQ_SIGNAL(clicked()),
	this,TQ_SLOT(addText()));

    connect(remove,TQ_SIGNAL(clicked()),
	this,TQ_SLOT(removeText()));

    connect(ok,TQ_SIGNAL(clicked()),
	this,TQ_SLOT(accept()));

    connect(cancel,TQ_SIGNAL(clicked()),
	this,TQ_SLOT(reject()));

    connect(languages,TQ_SIGNAL(activated(int)),
	this,TQ_SLOT(updateText()));

    connect(keys,TQ_SIGNAL(activated(int)),
	this,TQ_SLOT(updateText()));

    imageChanged();
}

ImageTextEditor::~ImageTextEditor()
{
}

void ImageTextEditor::imageChanged()
{
    languages->clear();
    keys->clear();
    text->clear();
    languages->insertItem("<any>");

    languages->insertStringList(image.textLanguages());
    keys->insertStringList(image.textKeys());

    updateText();
}

void ImageTextEditor::accept()
{
    storeText();
    TQDialog::accept();
}

void ImageTextEditor::updateText()
{
    storeText();
    newlang->setText(languages->currentText());
    newkey->setText(keys->currentText());
    TQString t = image.text(currKey(),currLang());

    text->setText(t);
}

TQString ImageTextEditor::currKey()
{
    return newkey->text();
}

TQString ImageTextEditor::currLang()
{
    TQString l = newlang->text();
    if ( l=="<any>" )
	l = TQString::null;
    return l;
}

TQString ImageTextEditor::currText()
{
    TQString t = text->text();
    if ( t.isNull() ) t = "";
    return t;
}


void ImageTextEditor::removeText()
{
    image.setText(currKey(),currLang(),TQString::null);
}

void ImageTextEditor::addText()
{
    storeText();
}

void ImageTextEditor::storeText()
{
    if ( currKey().length() > 0 ) {
	image.setText(currKey(),currLang(),currText());
    }
}
