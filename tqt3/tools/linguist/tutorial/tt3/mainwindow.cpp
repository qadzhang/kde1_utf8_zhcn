/****************************************************************
**
** Implementation of MainWindow class, translation tutorial 3
**
****************************************************************/

#include "mainwindow.h"
#include "printpanel.h"

#include <ntqaccel.h>
#include <ntqapplication.h>
#include <ntqmenubar.h>
#include <ntqmessagebox.h>
#include <ntqpopupmenu.h>

MainWindow::MainWindow( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    setCaption( tr("Troll Print 1.0") );

    PrintPanel *pp = new PrintPanel( this );
    setCentralWidget( pp );

    TQPopupMenu *file = new TQPopupMenu( this );
    file->insertItem( tr("E&xit"), tqApp, TQ_SLOT(quit()),
		      tr("Ctrl+Q", "Quit") );
    TQPopupMenu *help = new TQPopupMenu( this );
    help->insertItem( tr("&About"), this, TQ_SLOT(about()), Key_F1 );
    help->insertItem( tr("About &TQt"), this, TQ_SLOT(aboutTQt()) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("&Help"), help );
    menuBar()->setSeparator( TQMenuBar::InWindowsStyle );
}

void MainWindow::about()
{
    TQMessageBox::information( this, tr("About Troll Print 1.0"),
		   tr("Troll Print 1.0.\n\n"
		      "Copyright 1999 Macroshaft, Inc.") );
}

void MainWindow::aboutTQt()
{
    TQMessageBox::aboutTQt( this );
}
