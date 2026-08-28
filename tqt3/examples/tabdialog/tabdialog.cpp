/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"

#include <ntqvbox.h>
#include <ntqlabel.h>
#include <ntqlineedit.h>
#include <ntqdatetime.h>
#include <ntqbuttongroup.h>
#include <ntqcheckbox.h>
#include <ntqlistbox.h>
#include <ntqapplication.h>

TabDialog::TabDialog( TQWidget *parent, const char *name, const TQString &_filename )
    : TQTabDialog( parent, name ), filename( _filename ), fileinfo( filename )
{
    setupTab1();
    setupTab2();
    setupTab3();

    connect( this, TQ_SIGNAL( applyButtonPressed() ), tqApp, TQ_SLOT( quit() ) );
}

void TabDialog::setupTab1()
{
    TQVBox *tab1 = new TQVBox( this );
    tab1->setMargin( 5 );

    (void)new TQLabel( "Filename:", tab1 );
    TQLineEdit *fname = new TQLineEdit( filename, tab1 );
    fname->setFocus();

    (void)new TQLabel( "Path:", tab1 );
    TQLabel *path = new TQLabel( fileinfo.dirPath( true ), tab1 );
    path->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    (void)new TQLabel( "Size:", tab1 );
    ulong kb = (ulong)(fileinfo.size()/1024);
    TQLabel *size = new TQLabel( TQString( "%1 KB" ).arg( kb ), tab1 );
    size->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    (void)new TQLabel( "Last Read:", tab1 );
    TQLabel *lread = new TQLabel( fileinfo.lastRead().toString(), tab1 );
    lread->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    (void)new TQLabel( "Last Modified:", tab1 );
    TQLabel *lmodif = new TQLabel( fileinfo.lastModified().toString(), tab1 );
    lmodif->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    addTab( tab1, "General" );
}

void TabDialog::setupTab2()
{
    TQVBox *tab2 = new TQVBox( this );
    tab2->setMargin( 5 );

    TQButtonGroup *bg = new TQButtonGroup( 1, TQGroupBox::Horizontal, "Permissions", tab2 );

    TQCheckBox *readable = new TQCheckBox( "Readable", bg );
    if ( fileinfo.isReadable() )
        readable->setChecked( true );

    TQCheckBox *writable = new TQCheckBox( "Writeable", bg );
    if ( fileinfo.isWritable() )
        writable->setChecked( true );

    TQCheckBox *executable = new TQCheckBox( "Executable", bg );
    if ( fileinfo.isExecutable() )
        executable->setChecked( true );

    TQButtonGroup *bg2 = new TQButtonGroup( 2, TQGroupBox::Horizontal, "Owner", tab2 );

    (void)new TQLabel( "Owner", bg2 );
    TQLabel *owner = new TQLabel( fileinfo.owner(), bg2 );
    owner->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    (void)new TQLabel( "Group", bg2 );
    TQLabel *group = new TQLabel( fileinfo.group(), bg2 );
    group->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );

    addTab( tab2, "Permissions" );
}

void TabDialog::setupTab3()
{
    TQVBox *tab3 = new TQVBox( this );
    tab3->setMargin( 5 );
    tab3->setSpacing( 5 );
    
    (void)new TQLabel( TQString( "Open %1 with:" ).arg( filename ), tab3 );

    TQListBox *prgs = new TQListBox( tab3 );
    for ( unsigned int i = 0; i < 30; i++ ) {
        TQString prg = TQString( "Application %1" ).arg( i );
        prgs->insertItem( prg );
    }
    prgs->setCurrentItem( 3 );

    (void)new TQCheckBox( TQString( "Open files with the extension '%1' always with this application" ).arg( fileinfo.extension() ), tab3 );

    addTab( tab3, "Applications" );
}
