/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqapplication.h>
#include <ntqtranslator.h>
#include <ntqfileinfo.h>
#include <ntqmessagebox.h>
#include <ntqcheckbox.h>
#include <ntqvbox.h>
#include <ntqlayout.h>
#include <ntqbuttongroup.h>
#include <ntqpushbutton.h>
#include <ntqsignalmapper.h>
#include <ntqtextcodec.h>
#include <stdlib.h>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#endif

#include "mywidget.h"

//#define USE_I18N_FONT

class TQVDialog : public TQDialog {
public:
    TQVDialog(TQWidget *parent=0, const char *name=0, bool modal=false,
             WFlags f=0) : TQDialog(parent,name,modal,f)
    {
	TQVBoxLayout* vb = new TQVBoxLayout(this,8);
	vb->setAutoAdd(true);
	hb = 0;
	sm = new TQSignalMapper(this);
	connect(sm,TQ_SIGNAL(mapped(int)),this,TQ_SLOT(done(int)));
    }
    void addButtons( const TQString& cancel=TQString::null,
		    const TQString& ok=TQString::null,
		    const TQString& mid1=TQString::null,
		    const TQString& mid2=TQString::null,
		    const TQString& mid3=TQString::null)
    {
	addButton(ok.isNull() ? TQObject::tr("OK") : ok, 1);
	if ( !mid1.isNull() ) addButton(mid1,2);
	if ( !mid2.isNull() ) addButton(mid2,3);
	if ( !mid3.isNull() ) addButton(mid3,4);
	addButton(cancel.isNull() ? TQObject::tr("Cancel") : cancel, 0);
    }

    void addButton( const TQString& text, int result )
    {
	if ( !hb )
	    hb = new TQHBox(this);
	TQPushButton *c = new TQPushButton(text, hb);
	sm->setMapping(c,result);
	connect(c,TQ_SIGNAL(clicked()),sm,TQ_SLOT(map()));
    }

private:
    TQSignalMapper *sm;
    TQHBox *hb;
};

MyWidget* showLang(TQString lang)
{

    static TQTranslator *translator = 0;

    tqApp->setPalette(TQPalette(TQColor(220-rand()%64,220-rand()%64,220-rand()%64)));

    lang = "mywidget_" + lang + ".qm";
    TQFileInfo fi( lang );

    if ( !fi.exists() ) {
	TQMessageBox::warning( 0, "File error",
			      TQString("Cannot find translation for language: "+lang+
				      "\n(try eg. 'de', 'ko' or 'no')") );
	return 0;
    }
    if ( translator ) {
	tqApp->removeTranslator( translator );
	delete translator;
    }
    translator = new TQTranslator( 0 );
    translator->load( lang, "." );
    tqApp->installTranslator( translator );
    MyWidget *m = new MyWidget;
    m->setCaption("TQt Example - i18n - " + m->caption() );
    return m;
}

int main( int argc, char** argv )
{
    TQApplication app( argc, argv );

    const char* qm[]=
	{ "ar", "cs", "de", "el", "en", "eo", "fr", "it", "jp", "ko", "no", "ru", "sk", "zh", 0 };

#if defined(Q_OS_UNIX)
    srand( getpid() << 2 );
#endif

    TQString lang;
    if ( argc == 2 )
        lang = argv[1];

    if ( argc != 2 || lang == "all" ) {
	TQVDialog dlg(0,0,true);
	TQCheckBox* qmb[sizeof(qm)/sizeof(qm[0])];
	int r;
	if ( lang == "all" ) {
	    r = 2;
	} else {
	    TQButtonGroup *bg = new TQButtonGroup(4,TQt::Vertical,"Choose Locales",&dlg);
	    TQString loc = TQTextCodec::locale();
	    for ( int i=0; qm[i]; i++ ) {
		qmb[i] = new TQCheckBox((const char*)qm[i],bg);
		qmb[i]->setChecked( loc == qm[i] );
	    }
	    dlg.addButtons("Cancel","OK","All");
	    r = dlg.exec();
	}
	if ( r ) {
	    TQRect screen = tqApp->desktop()->availableGeometry();
	    bool tight = screen.width() < 1024;
	    int x=screen.left()+5;
	    int y=screen.top()+25;
	    for ( int i=0; qm[i]; i++ ) {
		if ( r == 2 || qmb[i]->isChecked() ) {
		    MyWidget* w = showLang((const char*)qm[i]);

		    if( w == 0 ) exit( 0 );
		    TQObject::connect(w, TQ_SIGNAL(closed()), tqApp, TQ_SLOT(quit()));
		    w->setGeometry(x,y,197,356);
		    w->show();
		    if ( tight ) {
			x += 8;
			y += 8;
		    } else {
			x += 205;
			if ( x > 1000 ) {
			    x = 5;
			    y += 384;
			}
		    }
		}
	    }
	} else {
            exit( 0 );
        }
    } else {
	TQString lang = argv[1];
	TQWidget* m = showLang(lang);
	app.setMainWidget( m );
	m->setCaption("TQt Example - i18n");
	m->show();
    }

#ifdef USE_I18N_FONT
    memorymanager->savePrerenderedFont(font.handle(),false);
#endif

    // While we run "all", kill them all
    return app.exec();

}
