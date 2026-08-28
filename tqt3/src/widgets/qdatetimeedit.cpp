/****************************************************************************
**
** Implementation of date and time edit classes
**
** Created : 001103
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "ntqdatetimeedit.h"

#ifndef TQT_NO_DATETIMEEDIT

#include "../kernel/qinternal_p.h"
#include "../kernel/qrichtext_p.h"
#include "ntqrangecontrol.h"
#include "ntqapplication.h"
#include "ntqpixmap.h"
#include "ntqapplication.h"
#include "ntqvaluelist.h"
#include "ntqstring.h"
#include "ntqstyle.h"

#if defined(TQ_WS_WIN)
#include "qt_windows.h"
#endif

#define TQDATETIMEEDIT_HIDDEN_CHAR '0'

class TQ_EXPORT TQNumberSection
{
public:
    TQNumberSection( int selStart = 0, int selEnd = 0, bool separat = true, int actual = -1 )
	: selstart( selStart ), selend( selEnd ), act( actual ), sep( separat )
    {}
    int selectionStart() const { return selstart; }
    void setSelectionStart( int s ) { selstart = s; }
    int selectionEnd() const { return selend; }
    void setSelectionEnd( int s ) { selend = s; }
    int width() const { return selend - selstart; }
    int index() const { return act; }
    bool separator() const { return sep; }
private:
    int selstart :12;
    int selend	 :12;
    int act	 :7;
    bool sep	 :1;
};

static TQString	*lDateSep = 0;
static TQString	*lTimeSep = 0;
static bool	lAMPM	  = false;
static TQString	*lAM	  = 0;
static TQString	*lPM	  = 0;
static TQDateEdit::Order	lOrder = TQDateEdit::YMD;
static int refcount = 0;

static void cleanup()
{
    delete lDateSep;
    lDateSep = 0;
    delete lTimeSep;
    lTimeSep = 0;
    delete lAM;
    lAM = 0;
    delete lPM;
    lPM = 0;
}

/*!
\internal
try to get the order of DMY and the date/time separator from the locale settings
*/
static void readLocaleSettings()
{
    int dpos, mpos, ypos;
    cleanup();

    lDateSep = new TQString();
    lTimeSep = new TQString();

#if defined(TQ_WS_WIN)
    QT_WA( {
	TCHAR data[10];
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SDATE, data, 10 );
	*lDateSep = TQString::fromUcs2( (ushort*)data );
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIME, data, 10 );
	*lTimeSep = TQString::fromUcs2( (ushort*)data );
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_ITIME, data, 10 );
	lAMPM = TQString::fromUcs2( (ushort*)data ).toInt()==0;
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S1159, data, 10 );
	TQString am = TQString::fromUcs2( (ushort*)data );
	if ( !am.isEmpty() )
	    lAM = new TQString( am );
	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_S2359, data, 10 );
	TQString pm = TQString::fromUcs2( (ushort*)data );
	if ( !pm.isEmpty()  )
	    lPM = new TQString( pm );
    } , {
	char data[10];
	GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SDATE, (char*)&data, 10 );
	*lDateSep = TQString::fromLocal8Bit( data );
	GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_STIME, (char*)&data, 10 );
	*lTimeSep = TQString::fromLocal8Bit( data );
	GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_ITIME, (char*)&data, 10 );
	lAMPM = TQString::fromLocal8Bit( data ).toInt()==0;
	GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_S1159, (char*)&data, 10 );
	TQString am = TQString::fromLocal8Bit( data );
	if ( !am.isEmpty() )
	    lAM = new TQString( am );
	GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_S2359, (char*)&data, 10 );
	TQString pm = TQString::fromLocal8Bit( data );
	if ( !pm.isEmpty() )
	    lPM = new TQString( pm );
    } );
#else
    *lDateSep = "-";
    *lTimeSep = ":";
#endif
    TQString d = TQDate( 1999, 11, 22 ).toString( TQt::LocalDate );
    dpos = d.find( "22" );
    mpos = d.find( "11" );
    ypos = d.find( "99" );
    if ( dpos > -1 && mpos > -1 && ypos > -1 ) {
	// test for DMY, MDY, YMD, YDM
	if ( dpos < mpos && mpos < ypos ) {
	    lOrder = TQDateEdit::DMY;
	} else if ( mpos < dpos && dpos < ypos ) {
	    lOrder = TQDateEdit::MDY;
	} else if ( ypos < mpos && mpos < dpos ) {
	    lOrder = TQDateEdit::YMD;
	} else if ( ypos < dpos && dpos < mpos ) {
	    lOrder = TQDateEdit::YDM;
	} else {
	    // cannot determine the dateformat - use the default
	    return;
	}

	// this code needs to change if new formats are added

#ifndef TQ_WS_WIN
	TQString sep = d.mid( TQMIN( dpos, mpos ) + 2, TQABS( dpos - mpos ) - 2 );
	if ( d.contains( sep ) == 2 ) {
	    *lDateSep = sep;
	}
#endif
    }

#ifndef TQ_WS_WIN
    TQString t = TQTime( 11, 22, 33 ).toString( TQt::LocalDate );
    dpos = t.find( "11" );
    mpos = t.find( "22" );
    ypos = t.find( "33" );
    // We only allow hhmmss
    if ( dpos > -1 && dpos < mpos && mpos < ypos ) {
	TQString sep = t.mid( dpos + 2, mpos - dpos - 2 );
	if ( sep == t.mid( mpos + 2, ypos - mpos - 2 ) ) {
	    *lTimeSep = sep;
	}
    }
#endif
}

static TQDateEdit::Order localOrder() {
    if ( !lDateSep ) {
	readLocaleSettings();
    }
    return lOrder;
}

static TQString localDateSep() {
    if ( !lDateSep ) {
	readLocaleSettings();
    }
    return *lDateSep;
}

static TQString localTimeSep() {
    if ( !lTimeSep ) {
	readLocaleSettings();
    }
    return *lTimeSep;
}

class TQDateTimeEditorPrivate
{
public:
    TQDateTimeEditorPrivate()
	: frm( true ),
	  parag( new TQTextParagraph( 0, 0, 0, false ) ),
	  focusSec(0)
    {
	parag->formatter()->setWrapEnabled( false );
	cursor = new TQTextCursor( 0 );
	cursor->setParagraph( parag );
	offset = 0;
	sep = localDateSep();
	refcount++;
    }
    ~TQDateTimeEditorPrivate()
    {
	delete parag;
	delete cursor;
	if ( !--refcount )
	    cleanup();
    }

    void appendSection( const TQNumberSection& sec )
    {
	sections.append( sec );

    }
    void clearSections()
    {
	sections.clear();
    }
    void setSectionSelection( int sec, int selstart, int selend )
    {
	if ( sec < 0 || sec > (int)sections.count() )
	    return;
	sections[sec].setSelectionStart( selstart );
	sections[sec].setSelectionEnd( selend );
    }
    uint sectionCount() const { return (uint)sections.count(); }
    void setSeparator( const TQString& s ) { sep = s; }
    TQString separator() const { return sep; }

    void setFrame( bool f ) { frm = f; }
    bool frame() const { return frm; }

    int focusSection() const { return focusSec; }
    int section( const TQPoint& p )
    {
	cursor->place( p + TQPoint( offset, 0 ), parag );
	int idx = cursor->index();
	for ( uint i = 0; i < sections.count(); ++i ) {
	    if ( idx >= sections[i].selectionStart() &&
		 idx <= sections[i].selectionEnd() )
		return i;
	}
	return -1;
    }
    TQNumberSection section( int idx ) const
    {
	return sections[idx];
    }
    bool setFocusSection( int idx )
    {
	if ( idx > (int)sections.count()-1 || idx < 0 )
	    return false;
	if ( idx != focusSec ) {
	    focusSec = idx;
	    applyFocusSelection();
	    return true;
	}
	return false;
    }

    bool inSectionSelection( int idx )
    {
	for ( uint i = 0; i < sections.count(); ++i ) {
	    if ( idx >= sections[i].selectionStart() &&
		 idx <= sections[i].selectionEnd() )
		return true;
	}
	return false;
    }

    void paint( const TQString& txt, bool focus, TQPainter& p,
		const TQColorGroup& cg, const TQRect& rect, TQStyle& style )
    {
	int fw = 0;
	if ( frm )
	    fw = style.pixelMetric(TQStyle::PM_DefaultFrameWidth);

	parag->truncate( 0 );
	parag->append( txt );
	if ( !focus )
	    parag->removeSelection( TQTextDocument::Standard );
	else {
	    applyFocusSelection();
	}

	/* color all TQDATETIMEEDIT_HIDDEN_CHAR chars to background color */
	TQTextFormat *fb = parag->formatCollection()->format( p.font(),
							     cg.base() );
	TQTextFormat *nf = parag->formatCollection()->format( p.font(),
							     cg.text() );
	for ( uint i = 0; i < txt.length(); ++i ) {
	    parag->setFormat( i, 1, nf );
	    if ( inSectionSelection( i ) )
		continue;
	    if ( txt.at(i) == TQDATETIMEEDIT_HIDDEN_CHAR )
		parag->setFormat( i, 1, fb );
	    else
		parag->setFormat( i, 1, nf );
	}
	fb->removeRef();
	nf->removeRef();

	TQRect r( rect.x(), rect.y(), rect.width() - 2 * ( 2 + fw ), rect.height() );
	parag->pseudoDocument()->docRect = r;
	parag->invalidate(0);
	parag->format();

	int xoff = 2 + fw - offset;
	int yoff = ( rect.height() - parag->rect().height() + 1 ) / 2;
	if ( yoff < 0 )
	    yoff = 0;

	p.translate( xoff, yoff );
	parag->paint( p, cg, 0, true );
	if ( frm )
	    p.translate( -xoff, -yoff );
    }

    void resize( const TQSize& size ) { sz = size; }

    int mapSection( int sec )
    {
	return sections[sec].index();
    }

protected:
    void applyFocusSelection()
    {
	if ( focusSec > -1 ) {
	    int selstart = sections[ focusSec ].selectionStart();
	    int selend = sections[ focusSec ].selectionEnd();
	    parag->setSelection( TQTextDocument::Standard, selstart, selend );
	    parag->format();
	    if ( parag->at( selstart )->x < offset ||
		 parag->at( selend )->x + parag->string()->width( selend ) > offset + sz.width() ) {
		offset = parag->at( selstart )->x;
	    }
	}
    }
private:
    bool frm;
    TQTextParagraph *parag;
    TQTextCursor *cursor;
    TQSize sz;
    int focusSec;
    TQValueList< TQNumberSection > sections;
    TQString sep;
    int offset;
};

class TQDateTimeEditor : public TQWidget
{
    TQ_OBJECT
public:
    TQDateTimeEditor( TQDateTimeEditBase * widget, TQWidget *parent,
		       const char * name=0 );
    ~TQDateTimeEditor();

    void setControlWidget( TQDateTimeEditBase * widget );
    TQDateTimeEditBase * controlWidget() const;

    void setSeparator( const TQString& s );
    TQString separator() const;

    int  focusSection() const;
    bool setFocusSection( int s );
    void appendSection( const TQNumberSection& sec );
    void clearSections();
    void setSectionSelection( int sec, int selstart, int selend );
    bool eventFilter( TQObject *o, TQEvent *e );
    int  sectionAt( const TQPoint &p );
    int mapSection( int sec );

protected:
    void init();
    bool event( TQEvent *e );
    void resizeEvent( TQResizeEvent * );
    void paintEvent( TQPaintEvent * );
    void mousePressEvent( TQMouseEvent *e );

private:
    TQDateTimeEditBase* cw;
    TQDateTimeEditorPrivate* d;
};

class TQDateTimeSpinWidget : public TQSpinWidget
{
public:
    TQDateTimeSpinWidget( TQWidget *parent, const char *name )
	: TQSpinWidget( parent, name )
    {
    }

    void enabledChange(bool notenabled)
    {
        TQDateEdit *de = ::tqt_cast<TQDateEdit*>(parentWidget());
        if (de && !notenabled) {
	    setUpEnabled(de->date() < de->maxValue());
	    setDownEnabled(de->date() > de->minValue());
        } else {
            setUpEnabled(!notenabled);
            setDownEnabled(!notenabled);
        }
    }

protected:
#ifndef TQT_NO_WHEELEVENT
    void wheelEvent( TQWheelEvent *e )
    {
	TQDateTimeEditor *editor = (TQDateTimeEditor*)editWidget()->tqt_cast( "TQDateTimeEditor" );
	Q_ASSERT( editor );
	if ( !editor )
	    return;

	int section = editor->sectionAt( e->pos() );
	editor->setFocusSection( section );

	if ( section == -1 )
	    return;
	TQSpinWidget::wheelEvent( e );
    }
#endif
};

/*!
    Constructs an empty datetime editor with parent \a parent and
    called \a name.
*/
TQDateTimeEditor::TQDateTimeEditor( TQDateTimeEditBase * widget, TQWidget *parent,
				  const char * name )
    : TQWidget( parent, name, WNoAutoErase )
{
    d = new TQDateTimeEditorPrivate();
    cw = widget;
    init();
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQDateTimeEditor::~TQDateTimeEditor()
{
    delete d;
}

/*! \internal

*/

void TQDateTimeEditor::init()
{
    setBackgroundMode( PaletteBase );
    setFocusSection( -1 );
    installEventFilter( this );
    setFocusPolicy( WheelFocus );
}


/*! \reimp

*/

bool TQDateTimeEditor::event( TQEvent *e )
{
    if ( e->type() == TQEvent::FocusIn || e->type() == TQEvent::FocusOut ) {
 	if ( e->type() == TQEvent::FocusOut )
  	    tqApp->sendEvent( cw, e );
	update( rect() );
    } else if ( e->type() == TQEvent::AccelOverride ) {
	TQKeyEvent* ke = (TQKeyEvent*) e;
	switch ( ke->key() ) {
	case Key_Delete:
	case Key_Backspace:
	case Key_Up:
	case Key_Down:
	case Key_Left:
	case Key_Right:
	    ke->accept();
	default:
	    break;
	}
    }
    return TQWidget::event( e );
}

/*! \reimp

*/

void TQDateTimeEditor::resizeEvent( TQResizeEvent *e )
{
    d->resize( e->size() );
    TQWidget::resizeEvent( e );
}


/*! \reimp

*/

void TQDateTimeEditor::paintEvent( TQPaintEvent * )
{
    TQString txt;
    for ( uint i = 0; i < d->sectionCount(); ++i ) {
	txt += cw->sectionFormattedText( i );
	if ( i < d->sectionCount()-1 ) {
	    if ( d->section( i+1 ).separator() )
		txt += d->separator();
	    else
		txt += " ";
	}
    }

    TQSharedDoubleBuffer buffer( this );
    const TQBrush &bg =
	colorGroup().brush( isEnabled() ? TQColorGroup::Base : TQColorGroup::Background );
    buffer.painter()->fillRect( 0, 0, width(), height(), bg );
    d->paint( txt, hasFocus(), *buffer.painter(), colorGroup(), rect(),
	      style() );
    buffer.end();
}


/*!
    Returns the section index at point \a p.
*/
int TQDateTimeEditor::sectionAt( const TQPoint &p )
{
    return d->section( p );
}

int TQDateTimeEditor::mapSection( int sec )
{
    return d->mapSection( sec );
}


/*! \reimp

*/

void TQDateTimeEditor::mousePressEvent( TQMouseEvent *e )
{
    TQPoint p( e->pos().x(), 0 );
    int sec = sectionAt( p );
    if ( sec != -1 ) {
	cw->setFocusSection( sec );
	repaint( rect(), false );
    }
}

/*! \reimp

*/
bool TQDateTimeEditor::eventFilter( TQObject *o, TQEvent *e )
{
    if ( o == this ) {
	if ( e->type() == TQEvent::KeyPress ) {
	    TQKeyEvent *ke = (TQKeyEvent*)e;
	    switch ( ke->key() ) {
	    case Key_Right:
		if ( d->focusSection() < (int)d->sectionCount()-1 ) {
		    if ( cw->setFocusSection( focusSection()+1 ) )
			repaint( rect(), false );
		}
		return true;
	    case Key_Left:
		if ( d->focusSection() > 0 ) {
		    if ( cw->setFocusSection( focusSection()-1 ) )
			repaint( rect(), false );
		}
		return true;
	    case Key_Up:
		cw->stepUp();
		return true;
	    case Key_Down:
		cw->stepDown();
		return true;
	    case Key_Backspace:
		if ( ::tqt_cast<TQDateEdit*>(cw) )
		    ((TQDateEdit*)cw)->removeFirstNumber( d->focusSection() );
		else if ( ::tqt_cast<TQTimeEdit*>(cw) )
		    ((TQTimeEdit*)cw)->removeFirstNumber( d->focusSection() );
		return true;
	    case Key_Delete:
		cw->removeLastNumber( d->focusSection() );
		return true;
	    case Key_Tab:
	    case Key_BackTab: {
		if ( ke->state() == TQt::ControlButton )
		    return false;

		TQWidget *w = this;
		bool hadDateEdit = false;
		while ( w ) {
		    if ( ( ::tqt_cast<TQDateTimeSpinWidget*>(w) && qstrcmp( w->name(), "qt_spin_widget" ) != 0 ) ||
			 ::tqt_cast<TQDateTimeEdit*>(w) )
			break;
		    hadDateEdit = hadDateEdit || ::tqt_cast<TQDateEdit*>(w);
		    w = w->parentWidget();
		}

		if ( w ) {
		    if ( !::tqt_cast<TQDateTimeEdit*>(w) ) {
			w = w->parentWidget();
		    } else {
			TQDateTimeEdit *ed = (TQDateTimeEdit*)w;
			if ( hadDateEdit && ke->key() == Key_Tab ) {
			    ed->timeEdit()->setFocus();
			    return true;
			} else if ( !hadDateEdit && ke->key() == Key_BackTab ) {
			    ed->dateEdit()->setFocus();
			    return true;
			} else {
			    while ( w && !::tqt_cast<TQDateTimeEdit*>(w) )
				w = w->parentWidget();
			}
		    }

		    tqApp->sendEvent( w, e );
		    return true;
		}
	    } break;
	    default:
		TQString txt = ke->text().lower();
		if ( !txt.isEmpty() && !separator().isEmpty() && txt[0] == separator()[0] ) {
		    // do the same thing as KEY_RIGHT when the user presses the separator key
		    if ( d->focusSection() < 2 ) {
			if ( cw->setFocusSection( focusSection()+1 ) )
			    repaint( rect(), false );
		    }
		    return true;
		} else if ( !txt.isEmpty() && ::tqt_cast<TQTimeEdit*>(cw) && focusSection() == (int) d->sectionCount()-1 ) {
		    // the first character of the AM/PM indicator toggles if the section has focus
		    TQTimeEdit *te = (TQTimeEdit*)cw;
		    TQTime time = te->time();
		    if ( lAMPM && lAM && lPM && (te->display()&TQTimeEdit::AMPM) ) {
			if ( txt[0] == (*lAM).lower()[0] && time.hour() >= 12 ) {
			    time.setHMS( time.hour()-12, time.minute(), time.second(), time.msec() );
			    te->setTime( time );
			} else if ( txt[0] == (*lPM).lower()[0] && time.hour() < 12 ) {
			    time.setHMS( time.hour()+12, time.minute(), time.second(), time.msec() );
			    te->setTime( time );
			}
		    }
		}

		int num = txt[0].digitValue();
		if ( num != -1 ) {
		    cw->addNumber( d->focusSection(), num );
		    return true;
		}
	    }
	}
    }
    return false;
}


/*!
    Appends the number section \a sec to the editor.
*/

void TQDateTimeEditor::appendSection( const TQNumberSection& sec )
{
    d->appendSection( sec );
}

/*!
    Removes all sections from the editor.
*/

void TQDateTimeEditor::clearSections()
{
    d->clearSections();
}

/*!
    Sets the selection of \a sec to start at \a selstart and end at \a
    selend.
*/

void TQDateTimeEditor::setSectionSelection( int sec, int selstart, int selend )
{
    d->setSectionSelection( sec, selstart, selend );
}

/*!
    Sets the separator for all numbered sections to \a s. Note that
    currently, only the first character of \a s is used.
*/

void TQDateTimeEditor::setSeparator( const TQString& s )
{
    d->setSeparator( s );
    update();
}


/*!
    Returns the editor's separator.
*/

TQString TQDateTimeEditor::separator() const
{
    return d->separator();
}

/*!
    Returns the number of the section that has focus.
*/

int TQDateTimeEditor::focusSection() const
{
    return d->focusSection();
}


/*!
    Sets the focus to section \a sec. If \a sec does not exist,
    nothing happens.
*/

bool TQDateTimeEditor::setFocusSection( int sec )
{
    return d->setFocusSection( sec );
}

/*! \class TQDateTimeEditBase
    \brief The TQDateTimeEditBase class provides an abstraction for date and edit editors.

    Small abstract class that provides some functions that are common
    for both TQDateEdit and TQTimeEdit. It is used internally by
    TQDateTimeEditor.
*/

/*!
    \fn TQDateTimeEditBase::TQDateTimeEditBase(TQWidget *, const char*)
    \internal
*/

/*!
    \fn TQDateTimeEditBase::setFocusSection(int)
    \internal
*/

/*! \fn TQString TQDateTimeEditBase::sectionFormattedText( int sec )
    \internal

  Pure virtual function which returns the formatted text of section \a
  sec.

*/

/*! \fn void TQDateTimeEditBase::stepUp()
    \internal

  Pure virtual slot which is called whenever the user increases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.
*/

/*! \fn void TQDateTimeEditBase::stepDown()
    \internal

  Pure virtual slot which is called whenever the user decreases the
  number in a section by pressing the widget's arrow buttons or the
  keyboard's arrow keys.

*/

/*! \fn void TQDateTimeEditBase::addNumber( int sec, int num )
    \internal

  Pure virtual function which is called whenever the user types a number.
  \a sec indicates the section where the number should be added. \a
  num is the number that was pressed.
*/

/*! \fn void TQDateTimeEditBase::removeLastNumber( int sec )
    \internal

  Pure virtual function which is called whenever the user tries to
  remove the last number from \a sec by pressing the delete key.
*/

////////////////

class TQDateEditPrivate
{
public:
    int y;
    int m;
    int d;
    // remebers the last entry for the day.
    // if the day is 31 and you cycle through the months,
    // the day will be 31 again if you reach a month with 31 days
    // otherwise it will be the highest day in the month
    int dayCache;
    int yearSection;
    int monthSection;
    int daySection;
    TQDateEdit::Order ord;
    bool overwrite;
    bool adv;
    int timerId;
    bool typing;
    TQDate min;
    TQDate max;
    bool changed;
    TQDateTimeEditor *ed;
    TQSpinWidget *controls;
};


/*!
    \class TQDateEdit ntqdatetimeedit.h
    \brief The TQDateEdit class provides a date editor.

    \ingroup advanced
    \ingroup time
    \mainclass

    TQDateEdit allows the user to edit dates by using the keyboard or
    the arrow keys to increase/decrease date values. The arrow keys
    can be used to move from section to section within the TQDateEdit
    box. Dates appear in accordance with the local date/time settings
    or in year, month, day order if the system doesn't provide this
    information. It is recommended that the TQDateEdit be initialised
    with a date, e.g.

    \code
    TQDateEdit *dateEdit = new TQDateEdit( TQDate::currentDate(), this );
    dateEdit->setRange( TQDate::currentDate().addDays( -365 ),
			TQDate::currentDate().addDays(  365 ) );
    dateEdit->setOrder( TQDateEdit::MDY );
    dateEdit->setAutoAdvance( true );
    \endcode

    Here we've created a new TQDateEdit object initialised with today's
    date and restricted the valid date range to today plus or minus
    365 days. We've set the order to month, day, year. If the auto
    advance property is true (as we've set it here) when the user
    completes a section of the date, e.g. enters two digits for the
    month, they are automatically taken to the next section.

    The maximum and minimum values for a date value in the date editor
    default to the maximum and minimum values for a TQDate. You can
    change this by calling setMinValue(), setMaxValue() or setRange().

    Terminology: A TQDateEdit widget comprises three 'sections', one
    each for the year, month and day. You can change the separator
    character using TQDateTimeEditor::setSeparator(), by default the
    separator will be taken from the systems settings. If that is
    not possible, it defaults to "-".

    \img datetimewidgets.png Date Time Widgets

    \sa TQDate TQTimeEdit TQDateTimeEdit
*/

/*!
    \enum TQDateEdit::Order

    This enum defines the order in which the sections that comprise a
    date appear.
    \value MDY month-day-year
    \value DMY day-month-year
    \value YMD year-month-day (the default)
    \value YDM year-day-month (included for completeness; but should
    not be used)
*/

/*!
    \enum TQTimeEdit::Display

    This enum defines the sections that comprise a time

    \value Hours The hours section
    \value Minutes The minutes section
    \value Seconds The seconds section
    \value AMPM The AM/PM section

    The values can be or'ed together to show any combination.
*/

/*!
    Constructs an empty date editor which is a child of \a parent and
    called name \a name.
*/

TQDateEdit::TQDateEdit( TQWidget * parent, const char * name )
    : TQDateTimeEditBase( parent, name )
{
    init();
    updateButtons();
}

/*!
    \overload

    Constructs a date editor with the initial value \a date, parent \a
    parent and called \a name.

    The date editor is initialized with \a date.
*/

TQDateEdit::TQDateEdit( const TQDate& date, TQWidget * parent, const char * name )
    : TQDateTimeEditBase( parent, name )
{
    init();
    setDate( date );
}

/*! \internal
*/
void TQDateEdit::init()
{
    d = new TQDateEditPrivate();
    d->controls = new TQDateTimeSpinWidget( this, qstrcmp( name(), "qt_datetime_dateedit" ) == 0 ? "qt_spin_widget" : "date edit controls" );
    d->ed = new TQDateTimeEditor( this, d->controls, "date editor" );
    d->controls->setEditWidget( d->ed );
    setFocusProxy( d->ed );
    connect( d->controls, TQ_SIGNAL( stepUpPressed() ), TQ_SLOT( stepUp() ) );
    connect( d->controls, TQ_SIGNAL( stepDownPressed() ), TQ_SLOT( stepDown() ) );
    connect( this, TQ_SIGNAL( valueChanged(const TQDate&) ),
	     TQ_SLOT( updateButtons() ) );
    d->ed->appendSection( TQNumberSection( 0,4 ) );
    d->ed->appendSection( TQNumberSection( 5,7 ) );
    d->ed->appendSection( TQNumberSection( 8,10 ) );

    d->yearSection = -1;
    d->monthSection = -1;
    d->daySection = -1;

    d->y = 0;
    d->m = 0;
    d->d = 0;
    d->dayCache = 0;
    setOrder( localOrder() );
    setFocusSection( 0 );
    d->overwrite = true;
    d->adv = false;
    d->timerId = 0;
    d->typing = false;
    d->min = TQDate( 1752, 9, 14 );
    d->max = TQDate( 8000, 12, 31 );
    d->changed = false;

    setSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed );

    refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQDateEdit::~TQDateEdit()
{
    delete d;
    if ( !--refcount )
	cleanup();
}

/*!
    \property TQDateEdit::minValue

    \brief the editor's minimum value

    Setting the minimum date value is equivalent to calling
    TQDateEdit::setRange( \e d, maxValue() ), where \e d is the minimum
    date. The default minimum date is 1752-09-14.

    \sa maxValue setRange()
*/

TQDate TQDateEdit::minValue() const
{
    return d->min;
}

/*!
    \property TQDateEdit::maxValue

    \brief the editor's maximum value

    Setting the maximum date value for the editor is equivalent to
    calling TQDateEdit::setRange( minValue(), \e d ), where \e d is the
    maximum date. The default maximum date is 8000-12-31.

    \sa minValue setRange()
*/

TQDate TQDateEdit::maxValue() const
{
    return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum date will be set.
    Similarly, if \a max is invalid no maximum date will be set.
*/

void TQDateEdit::setRange( const TQDate& min, const TQDate& max )
{
    if ( min.isValid() )
	d->min = min;
    if ( max.isValid() )
	d->max = max;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void TQDateEdit::setSeparator( const TQString& s )
{
    d->ed->setSeparator( s );
}

/*!
    Returns the editor's separator.
*/

TQString TQDateEdit::separator() const
{
    return d->ed->separator();
}


/*!
    Enables/disables the push buttons according to the min/max date
    for this widget.
*/

void TQDateEdit::updateButtons()
{
    if ( !isEnabled() )
	return;

    bool upEnabled = date() < maxValue();
    bool downEnabled = date() > minValue();

    d->controls->setUpEnabled( upEnabled );
    d->controls->setDownEnabled( downEnabled );
}

/*! \reimp
 */
void TQDateEdit::resizeEvent( TQResizeEvent * )
{
    d->controls->resize( width(), height() );
}

/*! \reimp

*/
TQSize TQDateEdit::sizeHint() const
{
    constPolish();
    TQFontMetrics fm( font() );
    int fw = style().pixelMetric( TQStyle::PM_DefaultFrameWidth, this );
    int h = TQMAX( fm.lineSpacing(), 14 ) + 2;
    int w = 2 + fm.width( '9' ) * 8 + fm.width( d->ed->separator() ) * 2 + d->controls->upRect().width() + fw * 4;

    return TQSize( w, TQMAX(h + fw * 2,20) ).expandedTo( TQApplication::globalStrut() );
}

/*! \reimp

*/
TQSize TQDateEdit::minimumSizeHint() const
{
    return sizeHint();
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the year, month or day section, depending on
    the current display order.

    \sa setOrder()
*/

TQString TQDateEdit::sectionFormattedText( int sec )
{
    TQString txt;
    txt = sectionText( sec );
    if ( d->typing && sec == d->ed->focusSection() )
	d->ed->setSectionSelection( sec, sectionOffsetEnd( sec ) - txt.length(),
			     sectionOffsetEnd( sec ) );
    else
	d->ed->setSectionSelection( sec, sectionOffsetEnd( sec ) - sectionLength( sec ),
			     sectionOffsetEnd( sec ) );
    txt = txt.rightJustify( sectionLength( sec ), TQDATETIMEEDIT_HIDDEN_CHAR );
    return txt;
}


/*!
    Returns the desired length (number of digits) of section \a sec.
    This will correspond to either the year, month or day section,
    depending on the current display order.

    \sa setOrder()
*/

int TQDateEdit::sectionLength( int sec ) const
{
    int val = 0;
    if ( sec == d->yearSection ) {
	val = 4;
    } else if ( sec == d->monthSection ) {
	val = 2;
    } else if ( sec == d->daySection ) {
	val = 2;
    }
    return val;
}

/*!
    Returns the text of section \a sec. This will correspond to either
    the year, month or day section, depending on the current display
    order.

    \sa setOrder()
*/

TQString TQDateEdit::sectionText( int sec ) const
{
    int val = 0;
    if ( sec == d->yearSection ) {
	val = d->y;
    } else if ( sec == d->monthSection ) {
	val = d->m;
    } else if ( sec == d->daySection ) {
	val = d->d;
    }
    return TQString::number( val );
}

/*! \internal

  Returns the end of the section offset \a sec.

*/

int TQDateEdit::sectionOffsetEnd( int sec ) const
{
    if ( sec == d->yearSection ) {
	switch( d->ord ) {
	case DMY:
	case MDY:
	    return sectionOffsetEnd( sec-1) + separator().length() + sectionLength( sec );
	case YMD:
	case YDM:
	    return sectionLength( sec );
	}
    } else if ( sec == d->monthSection ) {
	switch( d->ord ) {
	case DMY:
	case YDM:
	case YMD:
	    return sectionOffsetEnd( sec-1) + separator().length() + sectionLength( sec );
	case MDY:
	    return sectionLength( sec );
	}
    } else if ( sec == d->daySection ) {
	switch( d->ord ) {
	case DMY:
	    return sectionLength( sec );
	case YMD:
	case MDY:
	case YDM:
	    return sectionOffsetEnd( sec-1 ) + separator().length() + sectionLength( sec );
	}
    }
    return 0;
}


/*!
    \property TQDateEdit::order
    \brief the order in which the year, month and day appear

    The default order is locale dependent.

    \sa Order
*/

void TQDateEdit::setOrder( TQDateEdit::Order order )
{
    d->ord = order;
    switch( d->ord ) {
    case DMY:
	d->yearSection = 2;
	d->monthSection = 1;
	d->daySection = 0;
	break;
    case MDY:
	d->yearSection = 2;
	d->monthSection = 0;
	d->daySection = 1;
	break;
    case YMD:
	d->yearSection = 0;
	d->monthSection = 1;
	d->daySection = 2;
	break;
    case YDM:
	d->yearSection = 0;
	d->monthSection = 2;
	d->daySection = 1;
	break;
    }
    if ( isVisible() )
	d->ed->repaint( d->ed->rect(), false );
}


TQDateEdit::Order TQDateEdit::order() const
{
    return d->ord;
}


/*! \reimp

*/
void TQDateEdit::stepUp()
{
    int sec = d->ed->focusSection();
    bool accepted = false;
    if ( sec == d->yearSection ) {
	if ( !outOfRange( d->y+1, d->m, d->d ) ) {
	    accepted = true;
	    setYear( d->y+1 );
	}
    } else if ( sec == d->monthSection ) {
	if ( !outOfRange( d->y, d->m+1, d->d ) ) {
	    accepted = true;
	    setMonth( d->m+1 );
	}
    } else if ( sec == d->daySection ) {
	if ( !outOfRange( d->y, d->m, d->d+1 ) ) {
	    accepted = true;
	    setDay( d->d+1 );
	}
    }
    if ( accepted ) {
	d->changed = false;
	emit valueChanged( date() );
    }
    d->ed->repaint( d->ed->rect(), false );
}



/*! \reimp

*/

void TQDateEdit::stepDown()
{
    int sec = d->ed->focusSection();
    bool accepted = false;
    if ( sec == d->yearSection ) {
	if ( !outOfRange( d->y-1, d->m, d->d ) ) {
	    accepted = true;
	    setYear( d->y-1 );
	}
    } else if ( sec == d->monthSection ) {
	if ( !outOfRange( d->y, d->m-1, d->d ) ) {
	    accepted = true;
	    setMonth( d->m-1 );
	}
    } else if ( sec == d->daySection ) {
	if ( !outOfRange( d->y, d->m, d->d-1 ) ) {
	    accepted = true;
	    setDay( d->d-1 );
	}
    }
    if ( accepted ) {
	d->changed = false;
	emit valueChanged( date() );
    }
    d->ed->repaint( d->ed->rect(), false );
}

/*!
    Sets the year to \a year, which must be a valid year. The range
    currently supported is from 1752 to 8000.

    \sa TQDate
*/

void TQDateEdit::setYear( int year )
{
    if ( year < 1752 )
	year = 1752;
    if ( year > 8000 )
	year = 8000;
    if ( !outOfRange( year, d->m, d->d ) ) {
	d->y = year;
	setMonth( d->m );
	int tmp = d->dayCache;
	setDay( d->dayCache );
	d->dayCache = tmp;
    }
}


/*!
    Sets the month to \a month, which must be a valid month, i.e.
    between 1 and 12.
*/

void TQDateEdit::setMonth( int month )
{
    if ( month < 1 )
	month = 1;
    if ( month > 12 )
	month = 12;
    if ( !outOfRange( d->y, month, d->d ) ) {
	d->m = month;
	int tmp = d->dayCache;
	setDay( d->dayCache );
	d->dayCache = tmp;
    }
}


/*!
    Sets the day to \a day, which must be a valid day. The function
    will ensure that the \a day set is valid for the month and year.
*/

void TQDateEdit::setDay( int day )
{
    if ( day < 1 )
	day = 1;
    if ( day > 31 )
	day = 31;
    if ( d->m > 0 && d->y > 1752 ) {
	while ( !TQDate::isValid( d->y, d->m, day ) )
	    --day;
	if ( !outOfRange( d->y, d->m, day ) )
	    d->d = day;
    } else if ( d->m > 0 ) {
	if ( day > 0 && day < 32 ) {
	    if ( !outOfRange( d->y, d->m, day ) )
		d->d = day;
	}
    }
    d->dayCache = d->d;
}


/*!
    \property TQDateEdit::date
    \brief the editor's date value.

    If the date property is not valid, the editor displays all zeroes
    and TQDateEdit::date() will return an invalid date. It is strongly
    recommended that the editor is given a default date value (e.g.
    currentDate()). That way, attempts to set the date property to an
    invalid date will fail.

    When changing the date property, if the date is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void TQDateEdit::setDate( const TQDate& date )
{
    if ( !date.isValid() ) {
	d->y = 0;
	d->m = 0;
	d->d = 0;
	d->dayCache = 0;
    } else {
	if ( date > maxValue() || date < minValue() )
	    return;
	d->y = date.year();
	d->m = date.month();
	d->d = date.day();
	d->dayCache = d->d;
	emit valueChanged( date );
    }
    d->changed = false;
    d->ed->repaint( d->ed->rect(), false );
}

TQDate TQDateEdit::date() const
{
    if ( TQDate::isValid( d->y, d->m, d->d ) )
	return TQDate( d->y, d->m, d->d );
    return TQDate();
}

/*!  \internal

  Returns true if \a y, \a m, \a d is out of range, otherwise returns
  false.

  \sa setRange()

*/

bool TQDateEdit::outOfRange( int y, int m, int d ) const
{
    if ( TQDate::isValid( y, m, d ) ) {
        TQDate currentDate( y, m, d );
        return currentDate < minValue() || currentDate > maxValue();
    }
    return false; /* assume ok */
}

/*!  \reimp

*/

void TQDateEdit::addNumber( int sec, int num )
{
    if ( sec == -1 )
	return;
    killTimer( d->timerId );
    bool overwrite = false;
    bool accepted = false;
    d->typing = true;
    TQString txt;
    if ( sec == d->yearSection ) {
	txt = TQString::number( d->y );
	if ( d->overwrite || txt.length() == 4 ) {
	    accepted = true;
	    d->y = num;
	} else {
	    txt += TQString::number( num );
	    if ( txt.length() == 4  ) {
		int val = txt.toInt();
		if ( val < 1792 )
		    d->y = 1792;
		else if ( val > 8000 )
		    d->y = 8000;
		else if ( outOfRange( val, d->m, d->d ) )
		    txt = TQString::number( d->y );
		else {
		    accepted = true;
		    d->y = val;
		}
	    } else {
		accepted = true;
		d->y = txt.toInt();
	    }
	    if ( d->adv && txt.length() == 4 ) {
		d->ed->setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
    } else if ( sec == d->monthSection ) {
	txt = TQString::number( d->m );
	if ( d->overwrite || txt.length() == 2 ) {
	    accepted = true;
	    d->m = num;
	} else {
	    txt += TQString::number( num );
	    int temp = txt.toInt();
	    if ( temp > 12 )
		temp = num;
	    if ( outOfRange( d->y, temp, d->d ) )
		txt = TQString::number( d->m );
	    else {
		accepted = true;
		d->m = temp;
	    }
	    if ( d->adv && txt.length() == 2 ) {
		d->ed->setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
    } else if ( sec == d->daySection ) {
	txt = TQString::number( d->d );
	if ( d->overwrite || txt.length() == 2 ) {
	    accepted = true;
	    d->d = num;
	    d->dayCache = d->d;
	} else {
	    txt += TQString::number( num );
	    int temp = txt.toInt();
	    if ( temp > 31 )
		temp = num;
	    if ( outOfRange( d->y, d->m, temp ) )
		txt = TQString::number( d->d );
	    else {
		accepted = true;
		d->d = temp;
		d->dayCache = d->d;
	    }
	    if ( d->adv && txt.length() == 2 ) {
		d->ed->setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
    }
    if ( accepted ) {
	d->changed = false;
	emit valueChanged( date() );
    }
    d->overwrite = overwrite;
    d->timerId = startTimer( tqApp->doubleClickInterval()*4 );
    d->ed->repaint( d->ed->rect(), false );
}


/*! \reimp

*/

bool TQDateEdit::setFocusSection( int s )
{
    if ( s != d->ed->focusSection() ) {
	killTimer( d->timerId );
	d->overwrite = true;
	d->typing = false;
	fix(); // will emit valueChanged if necessary
    }
    return d->ed->setFocusSection( s );
}


/*!
    Attempts to fix any invalid date entries.

    The rules applied are as follows:

    \list
    \i If the year has four digits it is left unchanged.
    \i If the year has two digits, the year will be changed to four
    digits in the range current year - 70 to current year + 29.
    \i If the year has three digits in the range 100..999, the
    current millennium, i.e. 2000, will be added giving a year
    in the range 2100..2999.
    \i If the day or month is 0 then it will be set to 1 or the
    minimum valid day\month in the range.
    \endlist

*/

void TQDateEdit::fix()
{
    bool changed = false;
    int currentYear = TQDate::currentDate().year();
    int year = d->y;
    if ( year < 100 ) {
	int currentCentury = currentYear / 100;
	year += currentCentury * 100;
	if ( currentYear > year ) {
	    if ( currentYear > year + 70 )
		year += 100;
	} else {
	    if ( year >= currentYear + 30 )
		year -= 100;
	}
	changed = true;
    } else if ( year < 1000 ) {
	int currentMillennium = currentYear / 10;
	year += currentMillennium * 10;
	changed = true;
    } else if (d->d == 0) {
	d->d = 1;
	changed = true;
    } else if (d->m == 0) {
	d->m = 1;
	changed = true;
    }
    if ( outOfRange( year, d->m, d->d ) ) {
	if ( minValue().isValid() && date() < minValue() ) {
	    d->d =  minValue().day();
	    d->dayCache = d->d;
	    d->m = minValue().month();
	    d->y = minValue().year();
	}
	if ( date() > maxValue() ) {
	    d->d =  maxValue().day();
	    d->dayCache = d->d;
	    d->m = maxValue().month();
	    d->y = maxValue().year();
	}
	changed = true;
    } else if ( changed )
	setYear( year );
    if ( changed ) {
	emit valueChanged( date() );
	d->changed = false;
    }
}


/*! \reimp

*/

bool TQDateEdit::event( TQEvent *e )
{
    if( e->type() == TQEvent::FocusOut ) {
	d->typing = false;
	fix();
	// the following can't be done in fix() because fix() called
	// from all over the place and it will break the old behaviour
	if ( !TQDate::isValid( d->y, d->m, d->d ) ) {
	    d->dayCache = d->d;
	    int i = d->d;
	    for ( ; i > 0; i-- ) {
		d->d = i;
		if ( TQDate::isValid( d->y, d->m, d->d ) )
		    break;
	    }
	    d->changed = true;
	}
	if ( d->changed ) {
	    emit valueChanged( date() );
	    d->changed = false;
	}
    } else if ( e->type() == TQEvent::LocaleChange ) {
	readLocaleSettings();
	d->ed->setSeparator( localDateSep() );
	setOrder( localOrder() );
    }
    return TQDateTimeEditBase::event( e );
}

/*!
  \internal

  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void TQDateEdit::removeFirstNumber( int sec )
{
    if ( sec == -1 )
	return;
    TQString txt;
    if ( sec == d->yearSection ) {
	txt = TQString::number( d->y );
	txt = txt.mid( 1, txt.length() ) + "0";
	d->y = txt.toInt();
    } else if ( sec == d->monthSection ) {
	txt = TQString::number( d->m );
	txt = txt.mid( 1, txt.length() ) + "0";
	d->m = txt.toInt();
    } else if ( sec == d->daySection ) {
	txt = TQString::number( d->d );
	txt = txt.mid( 1, txt.length() ) + "0";
	d->d = txt.toInt();
	d->dayCache = d->d;
    }
    d->ed->repaint( d->ed->rect(), false );
}

/*! \reimp

*/

void TQDateEdit::removeLastNumber( int sec )
{
    if ( sec == -1 )
	return;
    TQString txt;
    if ( sec == d->yearSection ) {
	txt = TQString::number( d->y );
	txt = txt.mid( 0, txt.length()-1 );
	d->y = txt.toInt();
    } else if ( sec == d->monthSection ) {
	txt = TQString::number( d->m );
	txt = txt.mid( 0, txt.length()-1 );
	d->m = txt.toInt();
    } else if ( sec == d->daySection ) {
	txt = TQString::number( d->d );
	txt = txt.mid( 0, txt.length()-1 );
	d->d = txt.toInt();
	d->dayCache = d->d;
    }
    d->ed->repaint( d->ed->rect(), false );
}

/*!
    \property TQDateEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section

    If autoAdvance is true, the editor will automatically advance
    focus to the next date section if a user has completed a section.
    The default is false.
*/

void TQDateEdit::setAutoAdvance( bool advance )
{
    d->adv = advance;
}


bool TQDateEdit::autoAdvance() const
{
    return d->adv;
}

/*! \reimp
*/

void TQDateEdit::timerEvent( TQTimerEvent * )
{
    d->overwrite = true;
}

/*!
    \fn void TQDateEdit::valueChanged( const TQDate& date )

    This signal is emitted whenever the editor's value changes. The \a
    date parameter is the new value.
*/

///////////

class TQTimeEditPrivate
{
public:
    int h;
    int m;
    int s;
    uint display;
    bool adv;
    bool overwrite;
    int timerId;
    bool typing;
    TQTime min;
    TQTime max;
    bool changed;
    TQDateTimeEditor *ed;
    TQSpinWidget *controls;
};

/*!
    \class TQTimeEdit ntqdatetimeedit.h
    \brief The TQTimeEdit class provides a time editor.

    \ingroup advanced
    \ingroup time
    \mainclass

    TQTimeEdit allows the user to edit times by using the keyboard or
    the arrow keys to increase/decrease time values. The arrow keys
    can be used to move from section to section within the TQTimeEdit
    box. The user can automatically be moved to the next section once
    they complete a section using setAutoAdvance(). Times appear in
    hour, minute, second order. It is recommended that the TQTimeEdit
    is initialised with a time, e.g.
    \code
    TQTime timeNow = TQTime::currentTime();
    TQTimeEdit *timeEdit = new TQTimeEdit( timeNow, this );
    timeEdit->setRange( timeNow, timeNow.addSecs( 60 * 60 ) );
    \endcode
    Here we've created a TQTimeEdit widget set to the current time.
    We've also set the minimum value to the current time and the
    maximum time to one hour from now.

    The maximum and minimum values for a time value in the time editor
    default to the maximum and minimum values for a TQTime. You can
    change this by calling setMinValue(), setMaxValue() or setRange().

    Terminology: A TQTimeWidget consists of three sections, one each
    for the hour, minute and second. You can change the separator
    character using setSeparator(), by default the separator is read
    from the system's settings.

    \img datetimewidgets.png Date Time Widgets

    \sa TQTime TQDateEdit TQDateTimeEdit
*/


/*!
    Constructs an empty time edit with parent \a parent and called \a
    name.
*/

TQTimeEdit::TQTimeEdit( TQWidget * parent, const char * name )
    : TQDateTimeEditBase( parent, name )
{
    init();
}

/*!
    \overload

    Constructs a time edit with the initial time value, \a time,
    parent \a parent and called \a name.
*/

TQTimeEdit::TQTimeEdit( const TQTime& time, TQWidget * parent, const char * name )
    : TQDateTimeEditBase( parent, name )
{
    init();
    setTime( time );
}

/*! \internal
 */

void TQTimeEdit::init()
{
    d = new TQTimeEditPrivate();
    d->controls = new TQDateTimeSpinWidget( this, qstrcmp( name(), "qt_datetime_timeedit" ) == 0 ? "qt_spin_widget" : "time edit controls" );
    d->ed = new TQDateTimeEditor( this, d->controls, "time edit base" );
    d->controls->setEditWidget( d->ed );
    setFocusProxy( d->ed );
    connect( d->controls, TQ_SIGNAL( stepUpPressed() ), TQ_SLOT( stepUp() ) );
    connect( d->controls, TQ_SIGNAL( stepDownPressed() ), TQ_SLOT( stepDown() ) );

    d->ed->appendSection( TQNumberSection( 0,0, true, 0 ) );
    d->ed->appendSection( TQNumberSection( 0,0, true, 1 ) );
    d->ed->appendSection( TQNumberSection( 0,0, true, 2 ) );
    d->ed->setSeparator( localTimeSep() );

    d->h = 0;
    d->m = 0;
    d->s = 0;
    d->display = Hours | Minutes | Seconds;
    if ( lAMPM ) {
	d->display |= AMPM;
	d->ed->appendSection( TQNumberSection( 0,0, false, 3 ) );
    }
    d->adv = false;
    d->overwrite = true;
    d->timerId = 0;
    d->typing = false;
    d->min = TQTime( 0, 0, 0 );
    d->max = TQTime( 23, 59, 59 );
    d->changed = false;

    setSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed );

    refcount++;
}

/*!
    Destroys the object and frees any allocated resources.
*/

TQTimeEdit::~TQTimeEdit()
{
    delete d;
    if ( !--refcount )
	cleanup();
}

/*!
    \property TQTimeEdit::minValue
    \brief the minimum time value

    Setting the minimum time value is equivalent to calling
    TQTimeEdit::setRange( \e t, maxValue() ), where \e t is the minimum
    time. The default minimum time is 00:00:00.

    \sa maxValue setRange()
*/

TQTime TQTimeEdit::minValue() const
{
    return d->min;
}

/*!
    \property TQTimeEdit::maxValue
    \brief the maximum time value

    Setting the maximum time value is equivalent to calling
    TQTimeEdit::setRange( minValue(), \e t ), where \e t is the maximum
    time. The default maximum time is 23:59:59.

    \sa minValue setRange()
*/

TQTime TQTimeEdit::maxValue() const
{
    return d->max;
}


/*!
    Sets the valid input range for the editor to be from \a min to \a
    max inclusive. If \a min is invalid no minimum time is set.
    Similarly, if \a max is invalid no maximum time is set.
*/

void TQTimeEdit::setRange( const TQTime& min, const TQTime& max )
{
    if ( min.isValid() )
	d->min = min;
    if ( max.isValid() )
	d->max = max;
}

/*!
  \property TQTimeEdit::display
  \brief the sections that are displayed in the time edit

  The value can be any combination of the values in the Display enum.
  By default, the widget displays hours, minutes and seconds.
*/
void TQTimeEdit::setDisplay( uint display )
{
    if ( d->display == display )
	return;

    d->ed->clearSections();
    d->display = display;
    if ( d->display & Hours )
	d->ed->appendSection( TQNumberSection( 0,0, true, 0 ) );
    if ( d->display & Minutes )
	d->ed->appendSection( TQNumberSection( 0,0, true, 1 ) );
    if ( d->display & Seconds )
	d->ed->appendSection( TQNumberSection( 0,0, true, 2 ) );
    if ( d->display & AMPM )
	d->ed->appendSection( TQNumberSection( 0,0, false, 3 ) );

    d->ed->setFocusSection( 0 );
    d->ed->update();
}

uint TQTimeEdit::display() const
{
    return d->display;
}

/*!
    \property TQTimeEdit::time
    \brief the editor's time value.

    When changing the time property, if the time is less than
    minValue(), or is greater than maxValue(), nothing happens.
*/

void TQTimeEdit::setTime( const TQTime& time )
{
    if ( !time.isValid() ) {
	d->h = 0;
	d->m = 0;
	d->s = 0;
    } else {
	if ( time > maxValue() || time < minValue() )
	    return;
	d->h = time.hour();
	d->m = time.minute();
	d->s = time.second();
	emit valueChanged( time );
    }
    d->changed = false;
    d->ed->repaint( d->ed->rect(), false );
}

TQTime TQTimeEdit::time() const
{
    if ( TQTime::isValid( d->h, d->m, d->s ) )
	return TQTime( d->h, d->m, d->s );
    return TQTime();
}

/*!
    \property TQTimeEdit::autoAdvance
    \brief whether the editor automatically advances to the next
    section

    If autoAdvance is true, the editor will automatically advance
    focus to the next time section if a user has completed a section.
    The default is false.
*/

void TQTimeEdit::setAutoAdvance( bool advance )
{
    d->adv = advance;
}

bool TQTimeEdit::autoAdvance() const
{
    return d->adv;
}

/*!
    Sets the separator to \a s. Note that currently only the first
    character of \a s is used.
*/

void TQTimeEdit::setSeparator( const TQString& s )
{
    d->ed->setSeparator( s );
}

/*!
    Returns the editor's separator.
*/

TQString TQTimeEdit::separator() const
{
    return d->ed->separator();
}


/*!
    \fn void TQTimeEdit::valueChanged( const TQTime& time )

    This signal is emitted whenever the editor's value changes. The \a
    time parameter is the new value.
*/

/*! \reimp

*/

bool TQTimeEdit::event( TQEvent *e )
{
    if ( e->type() == TQEvent::FocusOut ) {
	d->typing = false;
	if ( d->changed ) {
	    emit valueChanged( time() );
	    d->changed = false;
	}
    } else if ( e->type() == TQEvent::LocaleChange ) {
	readLocaleSettings();
	d->ed->setSeparator( localTimeSep() );
    }
    return TQDateTimeEditBase::event( e );
}

/*! \reimp

*/

void TQTimeEdit::timerEvent( TQTimerEvent * )
{
    d->overwrite = true;
}


/*! \reimp

*/

void TQTimeEdit::stepUp()
{
    if (minValue() > maxValue()) {
        return;
    }
    int sec = d->ed->mapSection( d->ed->focusSection() );
    bool accepted = true;
    switch( sec ) {
    case 0:
        do {
            d->h = (d->h + 1) % 24;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 1:
        do {
            d->m = (d->m + 1) % 60;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 2:
        do {
            d->s = (d->s + 1) % 60;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 3:
	if ( d->h < 12 )
	    setHour( d->h+12 );
	else
	    setHour( d->h-12 );
	break;
    default:
	accepted = false;
#ifdef QT_CHECK_RANGE
	tqWarning( "TQTimeEdit::stepUp: Focus section out of range!" );
#endif
	break;
    }
    if ( accepted ) {
	d->changed = false;
	emit valueChanged( time() );
    }
    d->ed->repaint( d->ed->rect(), false );
}


/*! \reimp

*/

void TQTimeEdit::stepDown()
{
    if (minValue() > maxValue()) {
        return;
    }

    int sec = d->ed->mapSection( d->ed->focusSection() );
    bool accepted = true;
    switch( sec ) {
    case 0:
        do {
            if (--d->h < 0)
                d->h = 23;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 1:
        do {
            if (--d->m < 0)
                d->m = 59;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 2:
        do {
            if (--d->s < 0)
                d->s = 59;
        } while (outOfRange(d->h, d->m, d->s));
	break;
    case 3:
	if ( d->h > 11 )
	    setHour( d->h-12 );
	else
	    setHour( d->h+12 );
	break;
    default:
	accepted = false;
#ifdef QT_CHECK_RANGE
	tqWarning( "TQTimeEdit::stepDown: Focus section out of range!" );
#endif
	break;
    }
    if ( accepted ) {
	d->changed = false;
	emit valueChanged( time() );
    }
    d->ed->repaint( d->ed->rect(), false );
}


/*!
    Returns the formatted number for section \a sec. This will
    correspond to either the hour, minute or second section, depending
    on \a sec.
*/

TQString TQTimeEdit::sectionFormattedText( int sec )
{
    TQString txt;
    txt = sectionText( sec );
    txt = txt.rightJustify( 2, TQDATETIMEEDIT_HIDDEN_CHAR );
    int offset = sec*2+sec*separator().length() + txt.length();
    if ( d->typing && sec == d->ed->focusSection() )
	d->ed->setSectionSelection( sec, offset - txt.length(), offset );
    else
	d->ed->setSectionSelection( sec, offset - txt.length(), offset );

    return txt;
}


/*! \reimp

*/

bool TQTimeEdit::setFocusSection( int sec )
{
    if ( sec != d->ed->focusSection() ) {
	killTimer( d->timerId );
	d->overwrite = true;
	d->typing = false;
	TQString txt = sectionText( sec );
	txt = txt.rightJustify( 2, TQDATETIMEEDIT_HIDDEN_CHAR );
	int offset = sec*2+sec*separator().length() + txt.length();
	d->ed->setSectionSelection( sec, offset - txt.length(), offset );
	if ( d->changed ) {
	    emit valueChanged( time() );
	    d->changed = false;
	}
    }
    return d->ed->setFocusSection( sec );
}


/*!
    Sets the hour to \a h, which must be a valid hour, i.e. in the
    range 0..24.
*/

void TQTimeEdit::setHour( int h )
{
    if ( h < 0 )
	h = 0;
    if ( h > 23 )
	h = 23;
    d->h = h;
}


/*!
    Sets the minute to \a m, which must be a valid minute, i.e. in the
    range 0..59.
*/

void TQTimeEdit::setMinute( int m )
{
    if ( m < 0 )
	m = 0;
    if ( m > 59 )
	m = 59;
    d->m = m;
}


/*!
    Sets the second to \a s, which must be a valid second, i.e. in the
    range 0..59.
*/

void TQTimeEdit::setSecond( int s )
{
    if ( s < 0 )
	s = 0;
    if ( s > 59 )
	s = 59;
    d->s = s;
}


/*! \internal

  Returns the text of section \a sec.

*/

TQString TQTimeEdit::sectionText( int sec )
{
    sec = d->ed->mapSection( sec );

    TQString txt;
    switch( sec ) {
    case 0:
	if ( !(d->display & AMPM) || ( d->h < 13 && d->h ) ) {    // I wished the day stared at 0:00 for everybody
	    txt = TQString::number( d->h );
	} else {
	    if ( d->h )
		txt = TQString::number( d->h - 12 );
	    else
		txt = "12";
	}
	break;
    case 1:
	txt = TQString::number( d->m );
	break;
    case 2:
	txt = TQString::number( d->s );
	break;
    case 3:
	if ( d->h < 12 ) {
	    if ( lAM )
		txt = *lAM;
	    else
		txt = TQString::fromLatin1( "AM" );
	} else {
	    if ( lPM )
		txt = *lPM;
	    else
		txt = TQString::fromLatin1( "PM" );
	}
	break;
    default:
	break;
    }
    return txt;
}


/*! \internal
 Returns true if \a h, \a m, and \a s are out of range.
 */

bool TQTimeEdit::outOfRange( int h, int m, int s ) const
{
    if ( TQTime::isValid( h, m, s ) ) {
        TQTime currentTime( h, m, s );
        return currentTime < minValue() || currentTime > maxValue();
    }
    return true;
}

/*! \reimp

*/

void TQTimeEdit::addNumber( int sec, int num )
{
    if ( sec == -1 )
	return;
    sec = d->ed->mapSection( sec );
    killTimer( d->timerId );
    bool overwrite = false;
    bool accepted = false;
    d->typing = true;
    TQString txt;

    switch( sec ) {
    case 0:
	txt = ( d->display & AMPM && d->h > 12 ) ?
	    TQString::number( d->h - 12 ) : TQString::number( d->h );

	if ( d->overwrite || txt.length() == 2 ) {
	    if ( d->display & AMPM && num == 0 )
		break; // Don't process 0 in 12 hour clock mode
	    if ( d->display & AMPM && d->h > 11 )
		num += 12;
	    if ( !outOfRange( num, d->m, d->s ) ) {
		accepted = true;
		d->h = num;
	    }
	} else {
	    txt += TQString::number( num );
	    int temp = txt.toInt();

	    if ( d->display & AMPM ) {
		if ( temp == 12 ) {
		    if ( d->h < 12 ) {
			temp = 0;
		    }
		    accepted = true;
		} else if ( outOfRange( temp + 12, d->m, d->s ) ) {
		    txt = TQString::number( d->h );
		} else {
		    if ( d->h > 11 ) {
			temp += 12;
		    }
		    accepted = true;
		}
	    } else if ( !(d->display & AMPM) && outOfRange( temp, d->m, d->s ) ) {
		txt = TQString::number( d->h );
	    } else {
		accepted = true;
	    }

	    if ( accepted )
		d->h = temp;

	    if ( d->adv && txt.length() == 2 ) {
		setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
	break;

    case 1:
	txt = TQString::number( d->m );
	if ( d->overwrite || txt.length() == 2 ) {
	    if ( !outOfRange( d->h, num, d->s ) ) {
		accepted = true;
		d->m = num;
	    }
	} else {
	    txt += TQString::number( num );
	    int temp = txt.toInt();
	    if ( temp > 59 )
		temp = num;
	    if ( outOfRange( d->h, temp, d->s ) )
		txt = TQString::number( d->m );
	    else {
		accepted = true;
		d->m = temp;
	    }
	    if ( d->adv && txt.length() == 2 ) {
		setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
	break;

    case 2:
	txt = TQString::number( d->s );
	if ( d->overwrite || txt.length() == 2 ) {
	    if ( !outOfRange( d->h, d->m, num ) ) {
		accepted = true;
		d->s = num;
	    }
	} else {
	    txt += TQString::number( num );
	    int temp = txt.toInt();
	    if ( temp > 59 )
		temp = num;
	    if ( outOfRange( d->h, d->m, temp ) )
		txt = TQString::number( d->s );
	    else {
		accepted = true;
		d->s = temp;
	    }
	    if ( d->adv && txt.length() == 2 ) {
		setFocusSection( d->ed->focusSection()+1 );
		overwrite = true;
	    }
	}
	break;

    case 3:
	break;

    default:
	break;
    }
    d->changed = !accepted;
    if ( accepted )
	emit valueChanged( time() );
    d->overwrite = overwrite;
    d->timerId = startTimer( tqApp->doubleClickInterval()*4 );
    d->ed->repaint( d->ed->rect(), false );
}


/*!
  \internal

  Function which is called whenever the user tries to
  remove the first number from \a sec by pressing the backspace key.
*/

void TQTimeEdit::removeFirstNumber( int sec )
{
    if ( sec == -1 )
	return;
    sec = d->ed->mapSection( sec );
    TQString txt;
    switch( sec ) {
    case 0:
	txt = TQString::number( d->h );
	break;
    case 1:
	txt = TQString::number( d->m );
	break;
    case 2:
	txt = TQString::number( d->s );
	break;
    }
    txt = txt.mid( 1, txt.length() ) + "0";
    switch( sec ) {
    case 0:
	d->h = txt.toInt();
	break;
    case 1:
	d->m = txt.toInt();
	break;
    case 2:
	d->s = txt.toInt();
	break;
    }
    d->ed->repaint( d->ed->rect(), false );
}

/*! \reimp

*/
void TQTimeEdit::removeLastNumber( int sec )
{
    if ( sec == -1 )
	return;
    sec = d->ed->mapSection( sec );
    TQString txt;
    switch( sec ) {
    case 0:
	txt = TQString::number( d->h );
	break;
    case 1:
	txt = TQString::number( d->m );
	break;
    case 2:
	txt = TQString::number( d->s );
	break;
    }
    txt = txt.mid( 0, txt.length()-1 );
    switch( sec ) {
    case 0:
	d->h = txt.toInt();
	break;
    case 1:
	d->m = txt.toInt();
	break;
    case 2:
	d->s = txt.toInt();
	break;
    }
    d->ed->repaint( d->ed->rect(), false );
}

/*! \reimp
 */
void TQTimeEdit::resizeEvent( TQResizeEvent * )
{
    d->controls->resize( width(), height() );
}

/*! \reimp
*/
TQSize TQTimeEdit::sizeHint() const
{
    constPolish();
    TQFontMetrics fm( font() );
    int fw = style().pixelMetric( TQStyle::PM_DefaultFrameWidth, this );
    int h = fm.lineSpacing() + 2;
    int w = 2 + fm.width( '9' ) * 6 + fm.width( d->ed->separator() ) * 2 +
	d->controls->upRect().width() + fw * 4;
    if ( d->display & AMPM ) {
	if ( lAM )
	    w += fm.width( *lAM ) + 4;
	else
	    w += fm.width( TQString::fromLatin1( "AM" ) ) + 4;
    }

    return TQSize( w, TQMAX(h + fw * 2,20) ).expandedTo( TQApplication::globalStrut() );
}

/*! \reimp
*/
TQSize TQTimeEdit::minimumSizeHint() const
{
    return sizeHint();
}

/*!
    \internal
    Enables/disables the push buttons according to the min/max time
    for this widget.
*/

// ### Remove in 4.0?

void TQTimeEdit::updateButtons()
{
    if ( !isEnabled() )
	return;

    bool upEnabled = time() < maxValue();
    bool downEnabled = time() > minValue();

    d->controls->setUpEnabled( upEnabled );
    d->controls->setDownEnabled( downEnabled );
}


class TQDateTimeEditPrivate
{
public:
    bool adv;
};

/*!
    \class TQDateTimeEdit ntqdatetimeedit.h
    \brief The TQDateTimeEdit class combines a TQDateEdit and TQTimeEdit
    widget into a single widget for editing datetimes.

    \ingroup advanced
    \ingroup time
    \mainclass

    TQDateTimeEdit consists of a TQDateEdit and TQTimeEdit widget placed
    side by side and offers the functionality of both. The user can
    edit the date and time by using the keyboard or the arrow keys to
    increase/decrease date or time values. The Tab key can be used to
    move from section to section within the TQDateTimeEdit widget, and
    the user can be moved automatically when they complete a section
    using setAutoAdvance(). The datetime can be set with
    setDateTime().

    The date format is read from the system's locale settings. It is
    set to year, month, day order if that is not possible. See
    TQDateEdit::setOrder() to change this. Times appear in the order
    hours, minutes, seconds using the 24 hour clock.

    It is recommended that the TQDateTimeEdit is initialised with a
    datetime, e.g.
    \code
    TQDateTimeEdit *dateTimeEdit = new TQDateTimeEdit( TQDateTime::currentDateTime(), this );
    dateTimeEdit->dateEdit()->setRange( TQDateTime::currentDate(),
					TQDateTime::currentDate().addDays( 7 ) );
    \endcode
    Here we've created a new TQDateTimeEdit set to the current date and
    time, and set the date to have a minimum date of now and a maximum
    date of a week from now.

    Terminology: A TQDateEdit widget consists of three 'sections', one
    each for the year, month and day. Similarly a TQTimeEdit consists
    of three sections, one each for the hour, minute and second. The
    character that separates each date section is specified with
    setDateSeparator(); similarly setTimeSeparator() is used for the
    time sections.

    \img datetimewidgets.png Date Time Widgets

    \sa TQDateEdit TQTimeEdit
*/

/*!
    Constructs an empty datetime edit with parent \a parent and called
    \a name.
*/
TQDateTimeEdit::TQDateTimeEdit( TQWidget * parent, const char * name )
    : TQWidget( parent, name )
{
    init();
}


/*!
    \overload

    Constructs a datetime edit with the initial value \a datetime,
    parent \a parent and called \a name.
*/
TQDateTimeEdit::TQDateTimeEdit( const TQDateTime& datetime,
			      TQWidget * parent, const char * name )
    : TQWidget( parent, name )
{
    init();
    setDateTime( datetime );
}



/*!
    Destroys the object and frees any allocated resources.
*/

TQDateTimeEdit::~TQDateTimeEdit()
{
    delete d;
}


/*!
    \reimp

    Intercepts and handles resize events which have special meaning
    for the TQDateTimeEdit.
*/

void TQDateTimeEdit::resizeEvent( TQResizeEvent * )
{
    int dw = de->sizeHint().width();
    int tw = te->sizeHint().width();
    int w = width();
    int h = height();
    int extra = w - ( dw + tw );

    if ( tw + extra < 0 ) {
	dw = w;
    } else {
	dw += 9 * extra / 16;
    }
    tw = w - dw;

    de->setGeometry( 0, 0, dw, h );
    te->setGeometry( dw, 0, tw, h );
}

/*! \reimp
*/

TQSize TQDateTimeEdit::minimumSizeHint() const
{
    TQSize dsh = de->minimumSizeHint();
    TQSize tsh = te->minimumSizeHint();
    return TQSize( dsh.width() + tsh.width(),
		  TQMAX( dsh.height(), tsh.height() ) );
}

/*!  \internal
 */

void TQDateTimeEdit::init()
{
    d = new TQDateTimeEditPrivate();
    de = new TQDateEdit( this, "qt_datetime_dateedit" );
    te = new TQTimeEdit( this, "qt_datetime_timeedit" );
    d->adv = false;
    connect( de, TQ_SIGNAL( valueChanged(const TQDate&) ),
	     this, TQ_SLOT( newValue(const TQDate&) ) );
    connect( te, TQ_SIGNAL( valueChanged(const TQTime&) ),
	     this, TQ_SLOT( newValue(const TQTime&) ) );
    setFocusProxy( de );
    setSizePolicy( TQSizePolicy::Minimum, TQSizePolicy::Fixed );
}

/*! \reimp
 */

TQSize TQDateTimeEdit::sizeHint() const
{
    constPolish();
    TQSize dsh = de->sizeHint();
    TQSize tsh = te->sizeHint();
    return TQSize( dsh.width() + tsh.width(),
		  TQMAX( dsh.height(), tsh.height() ) );
}

/*!
    \property TQDateTimeEdit::dateTime
    \brief the editor's datetime value

    The datetime edit's datetime which may be an invalid datetime.
*/

void TQDateTimeEdit::setDateTime( const TQDateTime & dt )
{
    if ( dt.isValid() ) {
	de->setDate( dt.date() );
	te->setTime( dt.time() );
	emit valueChanged( dt );
    }
}

TQDateTime TQDateTimeEdit::dateTime() const
{
    return TQDateTime( de->date(), te->time() );
}

/*!
    \fn void TQDateTimeEdit::valueChanged( const TQDateTime& datetime )

    This signal is emitted every time the date or time changes. The \a
    datetime argument is the new datetime.
*/


/*! \internal

  Re-emits the value \a d.
 */

void TQDateTimeEdit::newValue( const TQDate& )
{
    TQDateTime dt = dateTime();
    emit valueChanged( dt );
}

/*! \internal
  \overload
  Re-emits the value \a t.
 */

void TQDateTimeEdit::newValue( const TQTime& )
{
    TQDateTime dt = dateTime();
    emit valueChanged( dt );
}


/*!
    Sets the auto advance property of the editor to \a advance. If set
    to true, the editor will automatically advance focus to the next
    date or time section if the user has completed a section.
*/

void TQDateTimeEdit::setAutoAdvance( bool advance )
{
    de->setAutoAdvance( advance );
    te->setAutoAdvance( advance );
}

/*!
    Returns true if auto-advance is enabled, otherwise returns false.

    \sa setAutoAdvance()
*/

bool TQDateTimeEdit::autoAdvance() const
{
    return de->autoAdvance();
}

/*!
    \fn TQDateEdit* TQDateTimeEdit::dateEdit()

    Returns the internal widget used for editing the date part of the
    datetime.
*/

/*!
    \fn TQTimeEdit* TQDateTimeEdit::timeEdit()

    Returns the internal widget used for editing the time part of the
    datetime.
*/

#include "qdatetimeedit.moc"

#endif
