/****************************************************************************
**
** Definition of date and time edit classes
**
** Created : 001103
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQDATETIMEEDIT_H
#define TQDATETIMEEDIT_H

#ifndef QT_H
#include "ntqwidget.h"
#include "ntqstring.h"
#include "ntqdatetime.h"
#endif // QT_H

#ifndef TQT_NO_DATETIMEEDIT

class TQ_EXPORT TQDateTimeEditBase : public TQWidget
{
    TQ_OBJECT
public:
    TQDateTimeEditBase( TQWidget* parent=0, const char* name=0 )
	: TQWidget( parent, name ) {}
    
    virtual bool setFocusSection( int sec ) = 0;
    virtual TQString sectionFormattedText( int sec ) = 0;
    virtual void addNumber( int sec, int num ) = 0;
    virtual void removeLastNumber( int sec ) = 0;

public slots:
    virtual void stepUp() = 0;
    virtual void stepDown() = 0;

private:
#if defined(TQ_DISABLE_COPY) // Disabled copy constructor and operator=
    TQDateTimeEditBase( const TQDateTimeEditBase & );
    TQDateTimeEditBase &operator=( const TQDateTimeEditBase & );
#endif
};

class TQDateEditPrivate;

class TQ_EXPORT TQDateEdit : public TQDateTimeEditBase
{
    TQ_OBJECT
    TQ_ENUMS( Order )
    TQ_PROPERTY( Order order READ order WRITE setOrder )
    TQ_PROPERTY( TQDate date READ date WRITE setDate )
    TQ_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
    TQ_PROPERTY( TQDate maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( TQDate minValue READ minValue WRITE setMinValue )

public:
    TQDateEdit( TQWidget* parent=0,  const char* name=0 );
    TQDateEdit( const TQDate& date, TQWidget* parent=0,  const char* name=0 );
    ~TQDateEdit();

    enum Order { DMY, MDY, YMD, YDM };

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void setDate( const TQDate& date );

public:
    TQDate date() const;
    virtual void setOrder( Order order );
    Order order() const;
    virtual void setAutoAdvance( bool advance );
    bool autoAdvance() const;

    virtual void setMinValue( const TQDate& d ) { setRange( d, maxValue() ); }
    TQDate minValue() const;
    virtual void setMaxValue( const TQDate& d ) { setRange( minValue(), d ); }
    TQDate maxValue() const;
    virtual void setRange( const TQDate& min, const TQDate& max );
    TQString separator() const;
    virtual void setSeparator( const TQString& s );

    // Make removeFirstNumber() virtual in TQDateTimeEditBase in 4.0
    void removeFirstNumber( int sec );

signals:
    void valueChanged( const TQDate& date );

protected:
    bool event( TQEvent *e );
    void timerEvent( TQTimerEvent * );
    void resizeEvent( TQResizeEvent * );
    void stepUp();
    void stepDown();
    TQString sectionFormattedText( int sec );
    void addNumber( int sec, int num );
    
    void removeLastNumber( int sec );
    bool setFocusSection( int s );

    virtual void setYear( int year );
    virtual void setMonth( int month );
    virtual void setDay( int day );
    virtual void fix();
    virtual bool outOfRange( int y, int m, int d ) const;

protected slots:
    void updateButtons();    

private:
    void init();
    int sectionOffsetEnd( int sec ) const;
    int sectionLength( int sec ) const;
    TQString sectionText( int sec ) const;
    TQDateEditPrivate* d;

#if defined(TQ_DISABLE_COPY)
    TQDateEdit( const TQDateEdit & );
    TQDateEdit &operator=( const TQDateEdit & );
#endif
};

class TQTimeEditPrivate;

class TQ_EXPORT TQTimeEdit : public TQDateTimeEditBase
{
    TQ_OBJECT
    TQ_SETS( Display )
    TQ_PROPERTY( TQTime time READ time WRITE setTime )
    TQ_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
    TQ_PROPERTY( TQTime maxValue READ maxValue WRITE setMaxValue )
    TQ_PROPERTY( TQTime minValue READ minValue WRITE setMinValue )
    TQ_PROPERTY( Display display READ display WRITE setDisplay )

public:
    enum Display {
	Hours	= 0x01,
	Minutes	= 0x02,
	Seconds	= 0x04,
	/*Reserved = 0x08,*/
	AMPM	= 0x10
    };

    TQTimeEdit( TQWidget* parent=0,  const char* name=0 );
    TQTimeEdit( const TQTime& time, TQWidget* parent=0,  const char* name=0 );
    ~TQTimeEdit();

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void setTime( const TQTime& time );

public:
    TQTime time() const;
    virtual void setAutoAdvance( bool advance );
    bool autoAdvance() const;

    virtual void setMinValue( const TQTime& d ) { setRange( d, maxValue() ); }
    TQTime minValue() const;
    virtual void setMaxValue( const TQTime& d ) { setRange( minValue(), d ); }
    TQTime maxValue() const;
    virtual void setRange( const TQTime& min, const TQTime& max );
    TQString separator() const;
    virtual void setSeparator( const TQString& s );

    uint display() const;
    void setDisplay( uint disp );

    // Make removeFirstNumber() virtual in TQDateTimeEditBase in 4.0
    void removeFirstNumber( int sec );

signals:
    void valueChanged( const TQTime& time );

protected:
    bool event( TQEvent *e );
    void timerEvent( TQTimerEvent *e );
    void resizeEvent( TQResizeEvent * );
    void stepUp();
    void stepDown();
    TQString sectionFormattedText( int sec );
    void addNumber( int sec, int num );
    void removeLastNumber( int sec );
    bool setFocusSection( int s );
    
    virtual bool outOfRange( int h, int m, int s ) const;
    virtual void setHour( int h );
    virtual void setMinute( int m );
    virtual void setSecond( int s );
    
protected slots:
    void updateButtons();

private:
    void init();
    TQString sectionText( int sec );
    TQTimeEditPrivate* d;

#if defined(TQ_DISABLE_COPY)
    TQTimeEdit( const TQTimeEdit & );
    TQTimeEdit &operator=( const TQTimeEdit & );
#endif
};


class TQDateTimeEditPrivate;

class TQ_EXPORT TQDateTimeEdit : public TQWidget
{
    TQ_OBJECT
    TQ_PROPERTY( TQDateTime dateTime READ dateTime WRITE setDateTime )

public:
    TQDateTimeEdit( TQWidget* parent=0, const char* name=0 );
    TQDateTimeEdit( const TQDateTime& datetime, TQWidget* parent=0,
		   const char* name=0 );
    ~TQDateTimeEdit();

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void setDateTime( const TQDateTime & dt );

public:
    TQDateTime dateTime() const;

    TQDateEdit* dateEdit() { return de; }
    TQTimeEdit* timeEdit() { return te; }

    virtual void setAutoAdvance( bool advance );
    bool autoAdvance() const;

signals:
    void valueChanged( const TQDateTime& datetime );

protected:
    // ### make init() private in TQt 4.0
    void init();
    void resizeEvent( TQResizeEvent * );

protected slots:
    // ### make these two functions private in TQt 4.0,
    //     and merge them into one with no parameter
    void newValue( const TQDate& d );
    void newValue( const TQTime& t );

private:
    TQDateEdit* de;
    TQTimeEdit* te;
    TQDateTimeEditPrivate* d;

#if defined(TQ_DISABLE_COPY)
    TQDateTimeEdit( const TQDateTimeEdit & );
    TQDateTimeEdit &operator=( const TQDateTimeEdit & );
#endif
};

#endif
#endif
