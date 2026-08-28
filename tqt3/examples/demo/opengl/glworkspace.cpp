#include "glworkspace.h"
#include "glbox.h"
#include "glgear.h"
#include "gltexobj.h"

#include <ntqworkspace.h>
#include <ntqdialog.h>
#include <ntqtoolbar.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqaction.h>
#include <ntqprinter.h>
#include <ntqpainter.h>
#include <ntqcheckbox.h>
#include <ntqvbox.h>
#include <ntqimage.h>
#include "printpreview.h"


GLWorkspace::GLWorkspace( TQWidget *parent, const char *name, WFlags f )
: TQMainWindow( parent, name, f ), printer( 0 )
{
    setupSceneActions();

    TQVBox *vbox = new TQVBox( this );
    vbox->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    vbox->setMargin( 1 );
    vbox->setLineWidth( 1 );

    workspace = new TQWorkspace( vbox );
    workspace->setBackgroundMode( PaletteMid );
    setCentralWidget( vbox );
}

GLWorkspace::~GLWorkspace()
{
}

void GLWorkspace::setupSceneActions()
{
    TQToolBar *tb = new TQToolBar( "Scene", this );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "&Scene" ), menu );

    TQAction *a;
    TQActionGroup *newGroup = new TQActionGroup( this );
    newGroup->setMenuText( tr( "&New" ) );
    newGroup->setText( tr( "New" ) );
    newGroup->setUsesDropDown( true );
    newGroup->setExclusive( false );
    newGroup->setIconSet( TQPixmap( "textdrawing/filenew.png" ) );
    a = new TQAction( tr( "Wirebox" ), TQPixmap( "opengl/wirebox.xpm" ), tr( "&Wirebox" ), 0, newGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( newWirebox() ) );
    a = new TQAction( tr( "Gear" ), TQPixmap( "opengl/gear.xpm" ), tr( "&Gears" ), 0, newGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( newGear() ) );
    a = new TQAction( tr( "Texture" ), TQPixmap( "opengl/texture.xpm" ), tr( "&Texture" ), 0, newGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( newTexture() ) );
/*    a = new TQAction( tr( "Nurbs" ), TQPixmap( "opengl/nurbs.xpm" ), tr( "&Nurbs" ), 0, newGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( newNurbs() ) );*/
    newGroup->addTo( tb );
    newGroup->addTo( menu );

    menu->insertSeparator();
    TQActionGroup *printGroup = new TQActionGroup( this );
    printGroup->setMenuText( tr( "&Print" ) );
    printGroup->setText( tr( "Print" ) );
    printGroup->setUsesDropDown( true );
    printGroup->setExclusive( false );
    printGroup->setIconSet( TQPixmap( "textdrawing/print.png" ) );
    TQAction *da = new TQAction( tr( "Window Size" ), TQPixmap( "textdrawing/print.png" ), tr( "&Window Size" ), CTRL + Key_P, printGroup );
    connect( da, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrintWindowRes() ) );
    a = new TQAction( tr( "Low Resolution" ), tr( "&Low Resolution" ), 0, printGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrintLowRes() ) );
    a = new TQAction( tr( "Medium Resolution" ), tr( "&Medium Resolution" ), 0, printGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrintMedRes() ) );
    a = new TQAction( tr( "High Resolution" ), tr( "&High Resolution" ), 0, printGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrintHighRes() ) );
    printGroup->addSeparator();
    a = new TQAction( tr( "Setup" ), tr( "&Setup..." ), 0, printGroup );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrintSetup() ) );
    da->addTo( tb );
    printGroup->addTo( menu );

    a = new TQAction( tr( "Close" ), TQPixmap(), tr( "&Close" ), 0, this );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileClose() ) );
    a->addTo( menu );
}

void GLWorkspace::newWirebox()
{
    GLBox *gl = new GLBox( workspace, 0, WDestructiveClose );
    gl->setIcon( TQPixmap( "opengl/wirebox.xpm" ) );
    gl->setCaption( tr( "Wirebox" ) );
    gl->resize( 320, 240 );
    gl->show();
}

void GLWorkspace::newGear()
{
    GLGear *gl = new GLGear( workspace, 0, WDestructiveClose );
    gl->setIcon( TQPixmap( "opengl/gear.xpm" ) );
    gl->setCaption( tr( "Gear" ) );
    gl->resize( 320, 240 );
    gl->show();
}

void GLWorkspace::newTexture()
{
    GLTexobj *gl = new GLTexobj( workspace, 0, WDestructiveClose );
    gl->setIcon( TQPixmap( "opengl/texture.xpm" ) );
    gl->setCaption( tr( "Texture" ) );
    gl->resize( 320, 240 );
    gl->show();
}

void GLWorkspace::newNurbs()
{
    GLGear *gl = new GLGear ( workspace, 0, WDestructiveClose );
    gl->setIcon( TQPixmap( "opengl/nurbs.xpm" ) );
    gl->setCaption( tr( "Nurbs" ) );
    gl->resize( 320, 240 );
    gl->show();
}

void GLWorkspace::filePrint( int x, int y )
{
    bool print = printer || filePrintSetup();
    if ( !print || !printer )
	return;

    TQWidget *widget = workspace->activeWindow();
    if ( !widget || !widget->inherits( "TQGLWidget" ) )
	return;
    TQGLWidget *gl = (TQGLWidget *)widget;
    TQPixmap pm = gl->renderPixmap( x, y );

    PrintPreview view( this );
    TQImage temp = pm.convertToImage();
    temp = temp.smoothScale( 400, 300 );
    TQPixmap temppix;
    temppix.convertFromImage( temp );
    view.setPixmap( temppix );
    view.setIcon( TQPixmap( "opengl/snapshot.xpm" ) );
    view.setCaption( gl->caption() + " - Print preview" );
    if ( view.exec() ) {
	TQImage img = pm.convertToImage();
	if ( view.checkInvert->isChecked() ) {
	    img.invertPixels();
	}
	if ( view.checkMirror->isChecked() ) {
	    img = img.mirror( true, false );
	}
	if ( view.checkFlip->isChecked() ) {
	    img = img.mirror( false, true );
	}
	if ( view.checkLeft->isEnabled() && view.checkLeft->isChecked() ) {
	}
	if ( view.checkRight->isEnabled() && view.checkRight->isChecked() ) {
	}
	pm.convertFromImage( img );

	TQPainter painter;
	if ( !painter.begin( printer ) )
	    return;

	painter.drawPixmap( TQPoint( 0, 0 ), pm );

	painter.end();
    }
}

void GLWorkspace::filePrintWindowRes()
{
    filePrint( 0, 0 );
}

void GLWorkspace::filePrintLowRes()
{
    filePrint( 640, 480 );
}

void GLWorkspace::filePrintMedRes()
{
    filePrint( 1024, 768 );
}

void GLWorkspace::filePrintHighRes()
{
    filePrint( 2048, 1536 );
}

bool GLWorkspace::filePrintSetup()
{
    bool newPrinter = !printer;

    if ( !printer )
	printer = new TQPrinter;
    if ( printer->setup() ) {
	return true;
    } else {
	if ( newPrinter ) {
	    delete printer;
	    printer = 0;
	}
	return false;
    }
}

void GLWorkspace::fileClose()
{
    workspace->closeActiveWindow();
}
