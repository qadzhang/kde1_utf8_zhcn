/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "frame.h"
#include "graph.h"
#include "display.h"
#include "icons.h"

#include "textdrawing/textedit.h"
#include "textdrawing/helpwindow.h"
#include "dnd/dnd.h"
#include "i18n/i18n.h"

#include <ntqmodules.h>

#if defined(TQT_MODULE_OPENGL)
#include "opengl/glworkspace.h"
#include "opengl/gllandscapeviewer.h"
#include "opengl/glinfotext.h"
#endif

#if defined(TQT_MODULE_CANVAS)
#include "qasteroids/toplevel.h"
#endif

#if defined(TQT_MODULE_TABLE)
#include "widgets/widgetsbase.h"
#else
#include "widgets/widgetsbase_pro.h"
#endif

#include <stdlib.h>

#include <ntqapplication.h>
#include <ntqimage.h>

#include <ntqtabwidget.h>
#include <ntqfont.h>
#include <ntqworkspace.h>
#include <ntqwidgetstack.h>

#if defined(TQT_MODULE_SQL)
#include <ntqsqldatabase.h>
#include "sql/sqlex.h"
#endif

#if defined(Q_OS_MACX)
#include <stdlib.h>
#include <ntqdir.h>
#endif

#include "categoryinterface.h"

static void qdemo_set_caption( CategoryInterface *c, int i )
{
    TQWidget *w = tqApp->mainWidget();
    if ( !w )
	return;
    TQString title = Frame::tr( "TQt Demo Collection" );
    title += " - " + c->categoryName( i - c->categoryOffset() );
    w->setCaption( title );
}

class WidgetCategory : public CategoryInterface
{
public:
    WidgetCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "Widgets"; }
    TQIconSet icon() const { return TQPixmap( widgeticon ); }
    int numCategories() const { return 2; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "Widgets" );
	    break;
	case 1:
	    return Frame::tr( "Drag and Drop" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new WidgetsBase( stack ), categoryOffset() + 0 );
	stack->addWidget( new DnDDemo( stack ), categoryOffset() + 1 );
    }

    int categoryOffset() const { return 0; }

private:
    bool created;

};

#if defined(TQT_MODULE_SQL)
class DatabaseCategory : public CategoryInterface
{
public:
    DatabaseCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "Database"; }
    TQIconSet icon() const { return TQPixmap( dbicon ); }
    int numCategories() const { return 1; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "SQL Explorer" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new SqlEx( stack ), categoryOffset() + 0 );
    }

    int categoryOffset() const { return 10; }

private:
    bool created;

};
#endif

#if defined(TQT_MODULE_CANVAS)
class CanvasCategory : public CategoryInterface
{
public:
    CanvasCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "2D Graphics"; }
    TQIconSet icon() const { return TQPixmap( twodicon ); }
    int numCategories() const { return 2; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "Graph Drawing" );
	    break;
	case 1:
	    return Frame::tr( "Display" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new GraphWidget( stack ), categoryOffset() + 0 );
	stack->addWidget( new DisplayWidget( stack ), categoryOffset() + 1 );
    }

    int categoryOffset() const { return 100; }

private:
    bool created;

};
#endif

#if defined(TQT_MODULE_OPENGL)
class OpenGLCategory : public CategoryInterface
{
public:
    OpenGLCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "3D Graphics"; }
    TQIconSet icon() const { return TQPixmap( threedicon ); }
    int numCategories() const { return 3; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "3D Demo" );
	    break;
	case 1:
	    return Frame::tr( "Fractal landscape" );
	    break;
	case 2:
	    return Frame::tr( "OpenGL info" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new GLWorkspace( stack ), categoryOffset() + 0 );
	stack->addWidget( new GLLandscapeViewer( stack ), categoryOffset() + 1 );
	stack->addWidget( new GLInfoText( stack ), categoryOffset() + 2 );
    }
    int categoryOffset() const { return 1000; }

private:
    bool created;

};
#endif

class TextCategory : public CategoryInterface
{
public:
    TextCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "Text Drawing/Editing"; }
    TQIconSet icon() const { return TQPixmap( texticon ); }
    int numCategories() const { return 2; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "Richtext Editor" );
	    break;
	case 1:
	    return Frame::tr( "Help Browser" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	TextEdit *te = new TextEdit( stack );
	te->load( "textdrawing/example.html" );
	stack->addWidget( te, categoryOffset() + 0 );
	TQString home = TQDir( "../../doc/html/index.html" ).absPath();
	HelpWindow *w = new HelpWindow( home, ".", stack, "helpviewer" );
	stack->addWidget( w, categoryOffset() + 1 );
    }
    int categoryOffset() const { return 10000; }

private:
    bool created;

};

class I18NCategory : public CategoryInterface
{
public:
    I18NCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "Internationalization"; }
    TQIconSet icon() const { return TQPixmap( internicon ); }
    int numCategories() const { return 1; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "Internationalization" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new I18nDemo( stack ), categoryOffset() + 0 );
    }
    int categoryOffset() const { return 100000; }

private:
    bool created;

};

#if defined(TQT_MODULE_CANVAS)
class GameCategory : public CategoryInterface
{
public:
    GameCategory( TQWidgetStack *s ) : CategoryInterface( s ), created( false ) {}

    TQString name() const { return "Game"; }
    TQIconSet icon() const { return TQPixmap( joyicon ); }
    int numCategories() const { return 1; }
    TQString categoryName( int i ) const {
	switch ( i ) {
	case 0:
	    return Frame::tr( "Asteroids" );
	    break;
	}
	return TQString::null;
    }
    TQIconSet categoryIcon( int ) const { return TQIconSet(); }
    void setCurrentCategory( int i ) {
	create();
	stack->raiseWidget( i );
	qdemo_set_caption( this, i );
    }
    void create() {
	if ( created )
	    return;
	created = true;
	stack->addWidget( new KAstTopLevel( stack ), categoryOffset() + 0 );
    }
    int categoryOffset() const { return 1000000; }

private:
    bool created;

};
#endif

int main( int argc, char **argv )
{
    TQString category;
    TQApplication a( argc, argv );

    Frame::updateTranslators();
    Frame frame;
    a.setMainWidget( &frame );

    TQPtrList<CategoryInterface> categories;
    categories.append( new WidgetCategory( frame.widgetStack() ) );
#if defined(TQT_MODULE_SQL)
    categories.append( new DatabaseCategory( frame.widgetStack() ) );
#endif
    categories.append( new CanvasCategory( frame.widgetStack() ) );
#if defined(TQT_MODULE_OPENGL)
    categories.append( new OpenGLCategory( frame.widgetStack() ) );
#endif
    categories.append( new TextCategory( frame.widgetStack() ) );
    categories.append( new I18NCategory( frame.widgetStack() ) );
    categories.append( new GameCategory( frame.widgetStack() ) );
    frame.setCategories( categories );

    frame.resize( 1000, 700 );
    frame.show();

    return a.exec();
}
