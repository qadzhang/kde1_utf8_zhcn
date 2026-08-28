
#include "viewer.h"
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqtextview.h>
#include <ntqpushbutton.h>
#include <ntqlayout.h>

Viewer::Viewer()
       :TQWidget()
{
    setFontSubstitutions();

    TQString greeting_heb = TQString::fromUtf8( "\327\251\327\234\327\225\327\235" );
    TQString greeting_ru = TQString::fromUtf8( "\320\227\320\264\321\200\320\260\320\262\321\201\321\202\320\262\321\203\320\271\321\202\320\265" );
    TQString greeting_en( "Hello" );

    greetings = new TQTextView( this, "textview" );

    greetings->setText( greeting_en + "\n" +
                       greeting_ru + "\n" +
                       greeting_heb );

    fontInfo = new TQTextView( this, "fontinfo" );

    setDefault();

    defaultButton = new TQPushButton( "Default", this,
                                                   "pushbutton1" );
    defaultButton->setFont( TQFont( "times" ) );
    connect( defaultButton, TQ_SIGNAL( clicked() ),
             this, TQ_SLOT( setDefault() ) );

    sansSerifButton = new TQPushButton( "Sans Serif", this,
                                                     "pushbutton2" );
    sansSerifButton->setFont( TQFont( "Helvetica", 12 ) );
    connect( sansSerifButton, TQ_SIGNAL( clicked() ),
             this, TQ_SLOT( setSansSerif() ) );

    italicsButton = new TQPushButton( "Italics", this,
                                                   "pushbutton3" );
    italicsButton->setFont( TQFont( "lucida", 12, TQFont::Bold, true ) );
    connect( italicsButton, TQ_SIGNAL( clicked() ),
             this, TQ_SLOT( setItalics() ) );

    layout();
}

void Viewer::setDefault()
{
    TQFont font( "Bavaria" );
    font.setPointSize( 24 );

    font.setWeight( TQFont::Bold );
    font.setUnderline( true );

    greetings->setFont( font );

    showFontInfo( font );
}

void Viewer::setSansSerif()
{
    TQFont font( "Newyork", 18 );
    font.setStyleHint( TQFont::SansSerif );
    greetings->setFont( font );

    showFontInfo( font );
}

void Viewer::setItalics()
{
    TQFont font( "Tokyo" );
    font.setPointSize( 32 );
    font.setWeight( TQFont::Bold );
    font.setItalic( true );

    greetings->setFont( font );

    showFontInfo( font );
}

void Viewer::showFontInfo( TQFont & font )
{
    TQFontInfo info( font );

    TQString messageText;
    messageText = "Font requested: \"" +
                  font.family() + "\" " +
                  TQString::number( font.pointSize() ) + "pt<BR>" +
                  "Font used: \"" +
                  info.family() + "\" " +
                  TQString::number( info.pointSize() ) + "pt<P>";

    TQStringList substitutions = TQFont::substitutes( font.family() );

    if ( ! substitutions.isEmpty() ){
	messageText += "The following substitutions exist for " + \
		       font.family() + ":<UL>";

	TQStringList::Iterator i = substitutions.begin();
	while ( i != substitutions.end() ){
	    messageText += "<LI>\"" + (* i) + "\"";
	    i++;
	}
	 messageText += "</UL>";
    } else {
	messageText += "No substitutions exist for " + \
		       font.family() + ".";
    }

    fontInfo->setText( messageText );
}

void Viewer::setFontSubstitutions()
{
    TQStringList substitutes;
    substitutes.append( "Times" );
    substitutes +=  "Mincho",
    substitutes << "Arabic Newspaper" << "crox";

    TQFont::insertSubstitutions( "Bavaria", substitutes );

    TQFont::insertSubstitution( "Tokyo", "Lucida" );
}


// For those who prefer to use TQt Designer for creating GUIs
// the following function might not be of particular interest:
// all it does is creating the widget layout.

void Viewer::layout()
{
    TQHBoxLayout * textViewContainer = new TQHBoxLayout();
    textViewContainer->addWidget( greetings );
    textViewContainer->addWidget( fontInfo );

    TQHBoxLayout * buttonContainer = new TQHBoxLayout();

    buttonContainer->addWidget( defaultButton );
    buttonContainer->addWidget( sansSerifButton );
    buttonContainer->addWidget( italicsButton );

    int maxButtonHeight = defaultButton->height();

    if ( sansSerifButton->height() > maxButtonHeight )
	maxButtonHeight = sansSerifButton->height();
    if ( italicsButton->height() > maxButtonHeight )
        maxButtonHeight = italicsButton->height();

    defaultButton->setFixedHeight( maxButtonHeight );
    sansSerifButton->setFixedHeight( maxButtonHeight );
    italicsButton->setFixedHeight( maxButtonHeight );

    TQVBoxLayout * container = new TQVBoxLayout( this );
    container->addLayout( textViewContainer );
    container->addLayout( buttonContainer );

    resize( 700, 250 );
}
