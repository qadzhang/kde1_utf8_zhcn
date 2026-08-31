/* This file is part of the KDE libraries
    Copyright (C) 1997 Martin Jones (mjones@kde.org)
              (C) 1997 Torben Weis (weis@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
//----------------------------------------------------------------------------
// khtml widget - forms
//
//

#include <qobject.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilinedit.h>
#include <qfontmetrics.h>
#include <qapp.h>
#include "htmlform.h"
#include <strings.h>
#include "htmlform.h"
#include "htmlfont.h"

//----------------------------------------------------------------------------


QString HTMLElement::encodeString( const QString &e )
{
	static char *safe = "$-._!*(),"; /* RFC 1738 */
	// 伪代码：
	//   1. 把 e 整串转成 UTF-8 字节流（TQCString）
	//   2. 逐字节扫描：
	//        a. 字母/数字/safe 字符 → 原样追加
	//        b. 空格 → '+'
	//        c. '\n' → "%0D%0A"（表单换行规范化为 CRLF）
	//        d. '\r' → 跳过
	//        e. 其余字节（含中文的多字节序列）→ %XX 十六进制转义
	// [2026-08-31] Why：原实现逐 TQChar 取单字节——非 Latin-1 字符（中文）
	//   高位被丢弃得 0，编出 "%00%00"（与 KURL::encodeURL 修复前同族缺陷）；
	//   现代浏览器对表单值的 percent-encoding 正是按 UTF-8 字节流进行的，
	//   本修法与其对齐（What's Here 表单提交中文可被现代站点正确解码）
	// Who：kfm 旧内核 HTML 表单 GET/POST 的 x-www-form-urlencoded 编码（17 调用点）
	// When：表单 submit 时对每个字段名/字段值调用
	TQCString utf = e.utf8();
	QString encoded;
	char buffer[5];

	for ( const char *p = utf.data(); p && *p; p++ )
	{
		unsigned char c = (unsigned char)*p;

		if ( (( c >= 'A') && ( c <= 'Z')) ||
		     (( c >= 'a') && ( c <= 'z')) ||
		     (( c >= '0') && ( c <= '9')) ||
		     (strchr(safe, c))
		   )
		{
			encoded += (char)c;
		}
		else if ( c == ' ' )
		{
			encoded += '+';
		}
		else if ( c == '\n' )
		{
			encoded += "%0D%0A";
		}
		else if ( c != '\r' )
		{
			sprintf( buffer, "%%%02X", (int)c );
			encoded += buffer;
		}
	}

	return encoded;
}

HTMLElement::~HTMLElement()
{
    if ( form )
	form->removeElement( this );
}

//----------------------------------------------------------------------------

HTMLWidgetElement::~HTMLWidgetElement()
{
    if ( widget )
	delete widget;
}

void HTMLWidgetElement::position( int _x, int _y, int , int _height )
{
	if ( widget == 0L ) // CC: HTMLHidden does not have a widget...
		return;

	if ( _y > absY() + ascent + descent || _y + _height < absY() )
	{
		widget->hide();
	}
	else
	{
		widget->move( absX() - _x, absY() - _y );
		widget->show();
	}
}

bool HTMLWidgetElement::positionChanged( int _x, int _y, int , int _height )
{
	if ( widget == 0L ) // CC: HTMLHidden does not have a widget...
		return false;

  int x = absX() - _x;
  int y = absY() - _y;

  if (x != widget->x() || y != widget->y())
  {
    return true;
  }

  return false;
}

void HTMLWidgetElement::calcAbsolutePos( int _x, int _y )
{
	_absX = _x + x;
	_absY = _y + y - ascent;
}

void HTMLWidgetElement::hideElement()
{
    if ( widget )
    {
        widget->hide();
    }
}

//----------------------------------------------------------------------------

HTMLSelect::HTMLSelect( QWidget *parent, const char *n, int s, bool m,
			const HTMLFont *f )
	: HTMLWidgetElement( n, f )
{
	_size = s;
	_defSelected = 0;
	_item = 0;

	_values.setAutoDelete( TRUE );

	QSize size;

	if ( _size > 1 )
	{
		widget = new QListBox( parent );
		size.setWidth( 150 );
		size.setHeight( 20 * _size );
		ascent = 25;
		descent = size.height() - ascent;
		((QListBox *)widget)->setMultiSelection( m );
	}
	else
	{
		widget = new QComboBox( FALSE, parent );
		size.setWidth( 150 );
		size.setHeight( 25 );
		descent = 5;
		ascent = size.height() - descent;
	}
	if( font )
	    widget->setFont( *font );

	connect( widget, SIGNAL( highlighted( int ) ),
			SLOT( slotHighlighted( int ) ) );

	widget->resize( size );

	width = size.width();
}

void HTMLSelect::addOption( const char *v, bool sel )
{
	if ( _size > 1 )
	{
		QListBox *lb = (QListBox *)widget;
		lb->insertItem( "" );
		if ( sel || lb->count() == 1 )
		{
			_defSelected = lb->count() - 1;
			lb->setSelected( _defSelected, true );
		}
		width = lb->maxItemWidth()+20;
		widget->resize( width, widget->height() );
	}
	else
	{
		QComboBox *cb = (QComboBox *)widget;
		cb->insertItem( "" );
		if ( sel || cb->count() == 1 )
		{
			_defSelected = cb->count() - 1;
			cb->setCurrentItem( _defSelected );
			_item = _defSelected;
		}
		QSize size = widget->sizeHint();
		widget->resize( size );
		ascent = size.height() - descent;
		width = size.width();
	}

	if ( v )
		_values.append( new QString( v ) );
	else
		_values.append( new QString( (char *)0L) );
}

const QString &HTMLSelect::value( int item )
{
	return *_values.at( item );
}

void HTMLSelect::setValue( const char *v, int indx )
{
	*_values.at( indx ) = v;
}

void HTMLSelect::setText( const char *text )
{
	int item;
	QString t = text;
	t = t.stripWhiteSpace();

	if ( _size > 1 )
	{
		QListBox *lb = (QListBox *)widget;
		lb->changeItem( t, lb->count() - 1 );
		item = lb->count() - 1;
		width = lb->maxItemWidth()+20;
		widget->resize( width, widget->height() );
	}
	else
	{
		QComboBox *cb = (QComboBox *)widget;
		cb->changeItem( t, cb->count() - 1 );
		item = cb->count() - 1;
		QSize size = widget->sizeHint();
		widget->resize( size );
		ascent = size.height() - descent;
		width = size.width();
	}

	if ( value( item ).isNull() )
		setValue( t, item );
}

QString HTMLSelect::encoding()
{
    QString _encoding = "";

    if ( elementName().length() )
    {
	if ( _size > 1 && ((QListBox *)widget)->isMultiSelection() )
	{ // multiple
	    QListBox* lb = (QListBox *) widget;

	    for ( unsigned i = 0; i < lb->count(); i++ )
	    {
		if ( lb->isSelected( i ) )
		{
		    if ( !_encoding.isEmpty() )
			_encoding += '&';
		    _encoding += encodeString( elementName() );
		    _encoding += '=';
		    _encoding += encodeString( value( i ) );
		}
	    }
	}
	else
	{
	    _encoding = encodeString( elementName() );
	    _encoding += '=';
	    _encoding += encodeString( value() );
	}
    }

    return _encoding;
}

void HTMLSelect::resetElement()
{
	if ( _size > 1 )
		((QListBox *)widget)->setCurrentItem( _defSelected );
	else
		((QComboBox *)widget)->setCurrentItem( _defSelected );
}

void HTMLSelect::slotHighlighted( int indx )
{
	_item = indx;
}

//----------------------------------------------------------------------------

HTMLTextArea::HTMLTextArea( QWidget *parent, const char *n, int r, int c,
			    const HTMLFont *f )
	: HTMLWidgetElement( n, f )
{
	_defText = "";

	widget = new QMultiLineEdit( parent );
	if( font )
	    widget->setFont( *font );

	QFontMetrics fm( widget->font() );

	// this is a bit better if using proportional fonts and
	// small inputs fields
	QSize size( c * fm.width('a') + 4, r * (fm.height()+1) );
	if( c < 5 )
	    size.setWidth( c * fm.width('M') + 4 );

	widget->resize( size );

	descent = size.height() - 14;
	ascent = 14;
	width = size.width();
}

QString HTMLTextArea::value()
{
	return ((QMultiLineEdit *)widget)->text();
}

void HTMLTextArea::setText( const char *t )
{
	_defText = t;
	((QMultiLineEdit *)widget)->setText( t );
}

QString HTMLTextArea::encoding()
{
	QString _encoding = "";

	if ( elementName().length() )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}
	
	return _encoding;
}

void HTMLTextArea::resetElement()
{
	((QMultiLineEdit *)widget)->setText( _defText );
}

//----------------------------------------------------------------------------

HTMLInput::HTMLInput( const char *n, const char *v, const HTMLFont *f )
	: HTMLWidgetElement( n, f )
{
	_value = v;
}

//----------------------------------------------------------------------------

HTMLButton::HTMLButton( KHTMLWidget *_parent, const char *_name, const char *v, QList<JSEventHandler> *_events, const HTMLFont *f )
	: HTMLInput( "", v, f )
{
    view = _parent;
    widget = new QPushButton( _parent );
    if( font )
	widget->setFont( *font );

    if ( strlen( value() ) != 0 )
	((QPushButton *)widget)->setText( value() );
    else if ( strlen( _name ) != 0 )
	((QPushButton *)widget)->setText( _name );
    else
	((QPushButton *)widget)->setText( "" );

    QSize size = widget->sizeHint();
    widget->resize( size );
    
    descent = 5;
    ascent = size.height() - descent;
    width = size.width();
    
    connect( widget, SIGNAL( clicked() ), SLOT( slotClicked() ) );
    
    eventHandlers = _events;
}

void HTMLButton::slotClicked()
{
    if ( eventHandlers == 0L )
	return;
    
    JSEventHandler* ev;
    for ( ev = eventHandlers->first(); ev != 0L; ev = eventHandlers->next() )
    {
	if ( strcmp( ev->getName(), "onClick" ) == 0L )
	{
	    ev->exec( 0L );
	    return;
	}
    }
}

HTMLButton::~HTMLButton()
{
    if ( eventHandlers )
	delete eventHandlers;
}

//----------------------------------------------------------------------------

HTMLCheckBox::HTMLCheckBox( QWidget *parent, const char *n, const char *v,
			    bool ch, const HTMLFont *f )
	: HTMLInput( n, v, f )
{
	_defCheck = ch;

	widget = new QCheckBox( parent );
	if( font )
	    widget->setFont( *font );

	((QCheckBox *)widget)->setChecked( ch );

	QSize size( 14, 14 );

	widget->resize( size );

	descent = 1;
	ascent = size.height() - descent;
	width = size.width() + 6;
}

QString HTMLCheckBox::encoding()
{
	QString _encoding = "";

	if ( ((QCheckBox *)widget)->isChecked() )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}

	return _encoding;
}

void HTMLCheckBox::resetElement()
{
	((QCheckBox *)widget)->setChecked( _defCheck );
}

//----------------------------------------------------------------------------

HTMLHidden::HTMLHidden( const char *n, const char *v )
	: HTMLInput( n, v )
{
}

QString HTMLHidden::encoding()
{
	QString _encoding;

	if ( elementName().length() )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}
	
	return _encoding;
}

//----------------------------------------------------------------------------

HTMLRadio::HTMLRadio( QWidget *parent, const char *n, const char *v,
		      bool ch, const HTMLFont *f )
	: HTMLInput( n, v, f )
{
	_defCheck = ch;

	widget = new QRadioButton( parent );
	if( font )
	    widget->setFont( *font );

	((QRadioButton *)widget)->setChecked( ch );

	QSize size( 14, 14 );

	widget->resize( size );

	descent = 1;
	ascent = size.height() - descent;
	width = size.width() + 6;

	connect( widget, SIGNAL( clicked() ), SLOT( slotClicked() ) );
}

QString HTMLRadio::encoding()
{
	QString _encoding = "";

	if ( ((QRadioButton *)widget)->isChecked() )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}

	return _encoding;
}

void HTMLRadio::resetElement()
{
	((QRadioButton *)widget)->setChecked( _defCheck );
}

void HTMLRadio::slotClicked()
{
	emit radioSelected( elementName(), value() );
}

void HTMLRadio::slotRadioSelected( const char *n, const char *v )
{
	if ( strcasecmp( n, elementName().data() ) != 0 )
		return;

	if ( strcasecmp( v, value().data() ) != 0 )
		((QRadioButton *)widget)->setChecked( false );
}

//----------------------------------------------------------------------------

HTMLReset::HTMLReset( QWidget *parent, const char *v, const HTMLFont *f )
	: HTMLInput( "", v, f )
{
	widget = new QPushButton( parent );
	if( font )
	    widget->setFont( *font );

	if ( strlen( value() ) != 0 )
		((QPushButton *)widget)->setText( value() );
	else
		((QPushButton *)widget)->setText( "Reset" );

	QSize size = widget->sizeHint();
	widget->resize( size );

	descent = 5;
	ascent = size.height() - descent;
	width = size.width();

	connect( widget, SIGNAL( clicked() ), SLOT( slotClicked() ) );
}

void HTMLReset::slotClicked()
{
	emit resetForm();
}

//----------------------------------------------------------------------------

HTMLSubmit::HTMLSubmit( QWidget *parent, const char *n, const char *v,
			const HTMLFont *f)
	: HTMLInput( n, v, f )
{
	widget = new QPushButton( parent );
	if( font )
	    widget->setFont( *font );

	if ( strlen( value() ) != 0 )
		((QPushButton *)widget)->setText( value() );
	else
		((QPushButton *)widget)->setText( "Submit Query" );

	QSize size = widget->sizeHint();
	widget->resize( size );

	descent = 5;
	ascent = size.height() - descent;
	width = size.width();

	connect( widget, SIGNAL( clicked() ), SLOT( slotClicked() ) );

	activated = false;
}

QString HTMLSubmit::encoding()
{
	QString _encoding = "";

	if ( elementName().length() && activated )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}

	return _encoding;
}

void HTMLSubmit::slotClicked()
{
	activated = true;
	emit submitForm();
}

//----------------------------------------------------------------------------

HTMLTextInput::HTMLTextInput( QWidget *parent, const char *n, const char *v,
			      int s, int ml, bool password, const HTMLFont *f )
	: HTMLInput( n, v, f )
{
	_defText = v;

	widget = new QLineEdit( parent );
	if( font )
	    widget->setFont( *font );

	if ( strlen( value() ) != 0 )
		((QLineEdit *)widget)->setText( value() );
	if ( password )
	    ((QLineEdit *)widget)->setEchoMode ( QLineEdit::Password );

	if ( ml > 0 )
	    ((QLineEdit *)widget)->setMaxLength( ml );

	QFontMetrics m = widget->fontMetrics();
	// this is a bit better if using proportional fonts and
	// small inputs fields
	QSize size( s * m.width('a') + 4, m.height() + 6);
	if(s<5)
	    size.setWidth( s * m.width('M') + 4);
	
	widget->resize( size );

	descent = 5;
	ascent = size.height() - descent;
	width = size.width();

	connect( widget, SIGNAL( textChanged( const char * ) ),
			SLOT( slotTextChanged( const char * ) ) );
	connect( widget, SIGNAL( returnPressed() ),
			SLOT( slotReturnPressed() ) );
}

QString HTMLTextInput::encoding()
{
	QString _encoding = "";

	if ( elementName().length() )
	{
		_encoding = encodeString( elementName() );
		_encoding += '=';
		_encoding += encodeString( value() );
	}

	return _encoding;
}

void HTMLTextInput::resetElement()
{
	((QLineEdit *)widget)->setText( _defText );
}

void HTMLTextInput::slotTextChanged( const char *t )
{
	setValue( t );
}

void HTMLTextInput::slotReturnPressed()
{
	emit submitForm();
}

//----------------------------------------------------------------------------

HTMLImageInput::HTMLImageInput( KHTMLWidget *widget, const char *f, int mw,
	const char *n )
    : HTMLImage( widget, f, 0, 0, mw ), HTMLElement( n )
{
    _xp = _yp = 0;
    pressed = false;
    activated = false;
}

QString HTMLImageInput::encoding()
{
    QString _encoding = "";

    if ( elementName().length() && activated )
    {
	QString num;

	num.setNum( _xp );
	_encoding = encodeString( elementName() );
	_encoding += ".x=";
	_encoding += num;

	_encoding += "&";

	num.setNum( _yp );
	_encoding += encodeString( elementName() );
	_encoding += ".y=";
	_encoding += num;
    }

    return _encoding;
}

HTMLObject *HTMLImageInput::mouseEvent( int _x, int _y, int button, int state )
{
    if ( _x < x || _x > x + width )
	return 0;

    if ( _y <= y - ascent || _y > y + descent )
	return 0;

    if ( button == LeftButton )
    {
	if ( ! ( state & LeftButton ) )
	{
	    pressed = true;
	}
	else if ( pressed )
	{
	    _xp = _x - x;
	    _yp = _y - ( y - ascent );
	    activated = true;
	    emit submitForm();
	    pressed = false;
	}

	return this;
    }

    return 0;
}

//----------------------------------------------------------------------------

HTMLForm::HTMLForm( const char *a, const char *m )
{
    _action = a;
    _method = m;

    elements.setAutoDelete( false );
    hidden.setAutoDelete( true );

    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(slotTimeout()) );
}

void HTMLForm::addElement( HTMLElement *e )
{
    elements.append( e );
}

void HTMLForm::addHidden( HTMLHidden *e )
{
    elements.append( e );
    hidden.append( e );
}

void HTMLForm::removeElement( HTMLElement *e )
{
    elements.removeRef( e );
}

void HTMLForm::position( int _x, int _y, int _width, int _height )
{
    if ( !timer->isActive() )
    {
        // Repositioning has started.  We first hide the elements, then
        // start a timer to perform the actual positioning once moving has
        // finished.  This makes scrolling a document with form controls
        // smoother.
        for ( HTMLElement *e = elements.first(); e != 0; e = elements.next() )
        {
          if (e->positionChanged(_x, _y, _width, _height))
            e->hideElement();
        }
    }
    else
    {
        timer->stop();
    }

    dx = _x;
    dy = _y;
    width = _width;
    height = _height;

    timer->start( 200, true );
}

void HTMLForm::slotReset()
{
    HTMLElement *e;

    for ( e = elements.first(); e != 0; e = elements.next() )
    {
	e->resetElement();
    }
}

void HTMLForm::slotSubmit()
{
	HTMLElement *e;
	QString encoding = "";
	bool first = true;

	for ( e = elements.first(); e != 0; e = elements.next() )
	{
		QString enc = e->encoding();
		if ( enc.length() )
		{
			if ( !first )
				encoding += '&';
			encoding += enc;
			first = false;
		}
	}

	QString url = action();

	emit submitted( method(), url, encoding );
}

void HTMLForm::slotRadioSelected( const char *n, const char *v )
{
    emit radioSelected( n, v );
}

void HTMLForm::slotTimeout()
{
    for ( HTMLElement *e = elements.first(); e != 0; e = elements.next() )
    {
        e->position( dx, dy, width, height );
    }
}

HTMLForm::~HTMLForm()
{
    HTMLElement *e;

    for ( e = elements.first(); e != 0; e = elements.next() )
    {
	e->setForm( 0 );
    }

    delete timer;
}
#include "htmlform.moc"
