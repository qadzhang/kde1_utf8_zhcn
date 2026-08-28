#include <ntqassistantclient.h>
#include <ntqmessagebox.h>
#include <ntqlineedit.h>
#include <ntqaction.h>
#include <ntqpopupmenu.h>
#include <ntqcheckbox.h>
#include <ntqprocess.h>
#include <ntqpushbutton.h>
#include <ntqdir.h>

#include "helpdemo.h"

HelpDemo::HelpDemo( TQWidget *parent, const char *name )
    : HelpDemoBase( parent, name )
{
    leFileName->setText( "./doc/index.html" );
    assistant = new TQAssistantClient( TQDir( "../../bin" ).absPath(), this );
    widgets.insert( (TQWidget*)openTQAButton, "./doc/manual.html#openqabutton" );
    widgets.insert( (TQWidget*)closeTQAButton, "./doc/manual.html#closeqabutton" );
    widgets.insert( (TQWidget*)checkOnlyExampleDoc, "./doc/manual.html#onlydoc" );
    widgets.insert( (TQWidget*)checkHide, "./doc/manual.html#hide" );
    widgets.insert( (TQWidget*)leFileName, "./doc/manual.html#lineedit" );
    widgets.insert( (TQWidget*)displayButton, "./doc/manual.html#displaybutton" );
    widgets.insert( (TQWidget*)closeButton, "./doc/manual.html#closebutton" );

    menu = new TQPopupMenu( this );

    TQAction *helpAction = new TQAction( "Show Help", TQKeySequence(tr("F1")), this );
    helpAction->addTo( menu );

    connect( helpAction, TQ_SIGNAL(activated()), this, TQ_SLOT(showHelp()) );
    connect( assistant, TQ_SIGNAL(assistantOpened()), this, TQ_SLOT(assistantOpened()) );
    connect( assistant, TQ_SIGNAL(assistantClosed()), this, TQ_SLOT(assistantClosed()));
    connect( assistant, TQ_SIGNAL(error(const TQString&)),
	     this, TQ_SLOT(showAssistantErrors(const TQString&)) );
    closeTQAButton->setEnabled(false);
}

HelpDemo::~HelpDemo()
{
}

void HelpDemo::contextMenuEvent( TQContextMenuEvent *e )
{
    TQWidget *w = lookForWidget();
    if ( menu->exec( e->globalPos() ) != -1 )
	showHelp( w );
}

TQWidget* HelpDemo::lookForWidget()
{
    TQPtrDictIterator<char> it( widgets );
    TQWidget *w;
    while ( (w = (TQWidget*)(it.currentKey())) != 0 ) {
	++it;
	if ( w->hasMouse() )
	    return w;
    }
    return 0;
}

void HelpDemo::showHelp()
{
    showHelp( lookForWidget() );  
}

void HelpDemo::showHelp( TQWidget *w )
{
    if ( w )
	assistant->showPage( TQString( widgets[w] ) );
    else
	assistant->showPage( "./doc/index.html" );
}

void HelpDemo::setAssistantArguments()
{
    TQStringList cmdLst;
    if ( checkHide->isChecked() )
	cmdLst << "-hideSidebar";
    if ( checkOnlyExampleDoc->isChecked() )
        cmdLst << "-profile"
	       << TQString("doc") + TQDir::separator() + TQString("helpdemo.adp");
    assistant->setArguments( cmdLst );
}

void HelpDemo::openAssistant()
{
    if ( !assistant->isOpen() )
	assistant->openAssistant();
}

void HelpDemo::closeAssistant()
{
    if ( assistant->isOpen() )
	assistant->closeAssistant();
}

void HelpDemo::displayPage()
{
    assistant->showPage( leFileName->text() );
}

void HelpDemo::showAssistantErrors( const TQString &err )
{
    TQMessageBox::critical( this, "Assistant Error", err );

}

void HelpDemo::assistantOpened()
{
    closeTQAButton->setEnabled( true );
    openTQAButton->setEnabled( false );
}

void HelpDemo::assistantClosed()
{
    closeTQAButton->setEnabled( false );
    openTQAButton->setEnabled( true );
}
