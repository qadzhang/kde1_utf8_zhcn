#include "i18n.h"
#include "wrapper.h"
#include "../textdrawing/textedit.h"

#include <ntqaction.h>
#include <ntqlayout.h>
#include <ntqvbox.h>
#include <ntqworkspace.h>
#include <ntqpopupmenu.h>
#include <ntqmenubar.h>
#include <ntqtoolbar.h>
#include <ntqtoolbutton.h>
#include <ntqpixmap.h>
#include <ntqiconset.h>
#include <ntqapplication.h>
#include <ntqwidgetlist.h>
#include <ntqlabel.h>
#include <ntqtextedit.h>


static int windowIdNumber = 5000;
static bool firstShow = true;


I18nDemo::I18nDemo(TQWidget *parent, const char *name)
    : TQMainWindow(parent, name, 0), lastwrapper(0)
{
    initActions();
    initMenuBar();

    TQVBox *box = new TQVBox(this);
    box->setFrameStyle( TQFrame::StyledPanel | TQFrame::Sunken );
    box->setMargin( 1 );
    box->setLineWidth( 1 );

    workspace = new TQWorkspace(box);
    connect(workspace, TQ_SIGNAL(windowActivated(TQWidget *)),
	    TQ_SLOT(windowActivated(TQWidget *)));
    workspace->setBackgroundMode(PaletteMid);

    setCentralWidget(box);
}


I18nDemo::~I18nDemo()
{
}


void I18nDemo::initActions()
{
    actionClose = new TQAction(tr("Close the current window."),
			      tr("Close"),
			      CTRL + Key_F4,
			      this);
    connect(actionClose, TQ_SIGNAL(activated()), TQ_SLOT(closeSlot()));

    actionCloseAll = new TQAction(tr("Close all opened windows."),
				 tr("Close All"),
				 0,
				 this);
    connect(actionCloseAll, TQ_SIGNAL(activated()), TQ_SLOT(closeAllSlot()));

    actionTile = new TQAction(tr("Tile opened windows."),
			     tr("Tile"),
			     0,
			     this);
    connect(actionTile, TQ_SIGNAL(activated()), TQ_SLOT(tileSlot()));

    actionCascade = new TQAction(tr("Cascade opened windows."),
				tr("Cascade"),
				0,
				this);
    connect(actionCascade, TQ_SIGNAL(activated()), TQ_SLOT(cascadeSlot()));
}


void I18nDemo::initMenuBar()
{
    newMenu = new TQPopupMenu(this);
    connect(newMenu, TQ_SIGNAL(activated(int)), TQ_SLOT(newSlot(int)));

    newMenu->insertItem("&English", 0);
    newMenu->insertItem("&Japanese", 1);
    newMenu->insertItem("&Korean", 2);
    newMenu->insertItem("&Norwegian", 3);

    windowMenu = new TQPopupMenu(this);
    connect(windowMenu, TQ_SIGNAL(activated(int)), TQ_SLOT(windowSlot(int)));

    windowMenu->setCheckable(true);

    actionClose->addTo(windowMenu);
    actionCloseAll->addTo(windowMenu);
    windowMenu->insertSeparator();
    actionTile->addTo(windowMenu);
    actionCascade->addTo(windowMenu);
    windowMenu->insertSeparator();

    menuBar()->insertItem(tr("&New"), newMenu);
    menuBar()->insertItem(tr("&Window"), windowMenu);
}


void I18nDemo::newSlot(int id)
{
    TQString qmfile;
    switch (id) {
    default:
    case 0: qmfile = "i18n/en.qm"; break;
    case 1: qmfile = "i18n/ja.qm"; break;
    case 2: qmfile = "i18n/ko.qm"; break;
    case 3: qmfile = "i18n/no.qm"; break;
    }

    if (lastwrapper) {
	tqApp->removeTranslator(&lastwrapper->translator);
	lastwrapper = 0;
    }

    Wrapper *wrapper = new Wrapper(workspace, windowIdNumber);
    wrapper->translator.load(qmfile, ".");

    tqApp->installTranslator(&wrapper->translator);

    connect(wrapper, TQ_SIGNAL(destroyed()), TQ_SLOT(wrapperDead()));
    wrapper->setCaption(tr("--language--"));

    TextEdit *te = new TextEdit(wrapper);
    te->layout()->setResizeMode( TQLayout::FreeResize );
    te->setMinimumSize(500, 400);
    te->fileNew();
    te->currentEditor()->
	setText(tr("<h3>About TQt</h3>"
		   "<p>This program uses TQt version %1, a multiplatform C++ "
		   "GUI toolkit from Trolltech. TQt provides single-source "
		   "portability across Windows 95/98/NT/2000, Mac OS X, Linux, Solaris "
		   "and many other versions of Unix with X11.</p>"
		   "<p>See <tt>http://www.trolltech.com/qt/</tt> for more "
		   "information.</p>").arg(TQT_VERSION_STR));

    tqApp->removeTranslator(&wrapper->translator);

    te->show();
    wrapper->show();

    windowMenu->insertItem(wrapper->caption(), wrapper->id);
    windowMenu->setItemChecked(wrapper->id, true);
    lastwrapper = wrapper;

    windowIdNumber++;
}


void I18nDemo::windowSlot(int id)
{
    if (id < 5000)
	return;

    TQWidgetList list = workspace->windowList();
    Wrapper *wrapper = (Wrapper *) list.first();
    while (wrapper) {
	if (wrapper->id == id) {
	    wrapper->setFocus();
	    break;
	}

	wrapper = (Wrapper *) list.next();
    }
}


void I18nDemo::windowActivated(TQWidget *w)
{
    if (lastwrapper) {
	tqApp->removeTranslator(&lastwrapper->translator);
	windowMenu->setItemChecked(lastwrapper->id, false);
    }

    if (! w) {
	lastwrapper = 0;
	return;
    }

    Wrapper *wrapper = (Wrapper *) w;

    windowMenu->setItemChecked(wrapper->id, true);
    lastwrapper = wrapper;
}


void I18nDemo::closeSlot()
{
    TQWidget *w = workspace->activeWindow();
    delete w;
}


void I18nDemo::closeAllSlot()
{
    TQWidget *w;
    while ((w = workspace->activeWindow()))
	w->close(true);
}


void I18nDemo::tileSlot()
{
    workspace->tile();
}


void I18nDemo::cascadeSlot()
{
    workspace->cascade();
}


void I18nDemo::wrapperDead()
{
    Wrapper *w = (Wrapper *) sender();

    if (w == lastwrapper) {
	tqApp->removeTranslator(&w->translator);
	lastwrapper = 0;
    }

    windowMenu->removeItem(w->id);
}


void I18nDemo::showEvent(TQShowEvent *)
{
    if (firstShow) {
	newSlot(1);
	firstShow = false;
	return;
    }

    if (! lastwrapper)
	return;

    tqApp->installTranslator(&lastwrapper->translator);
}


void I18nDemo::hideEvent(TQHideEvent *)
{
    if (! lastwrapper)
	return;

    tqApp->removeTranslator(&lastwrapper->translator);
}
