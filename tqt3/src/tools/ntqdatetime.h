/*************************************************************************
**
** Definition of date and time classes
**
** Created : 940124
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQDATETIME_H
#define TQDATETIME_H

#ifndef QT_H
#include "ntqstring.h"
#include "ntqnamespace.h"
#endif // QT_H

#include <time.h>

/*****************************************************************************
  TQDate class
 *****************************************************************************/

class TQ_EXPORT TQDate
{
public:
    TQDate() { jd = 0; }
    TQDate( int y, int m, int d );

    bool isNull() const { return jd == 0; }
    bool isValid() const;

    int year() const;
    int month() const;
    int day() const;
    int dayOfWeek() const;
    int dayOfYear() const;
    int daysInMonth() const;
    int daysInYear() const;
    int weekNumber( int *yearNum = 0 ) const;

#ifndef TQT_NO_TEXTDATE
#ifndef TQT_NO_COMPAT
    static TQString monthName( int month ) { return shortMonthName( month ); }
    static TQString dayName( int weekday ) { return shortDayName( weekday ); }
#endif
    static TQString shortMonthName( int month );
    static TQString shortDayName( int weekday );
    static TQString longMonthName( int month );
    static TQString longDayName( int weekday );
#endif //TQT_NO_TEXTDATE
#ifndef TQT_NO_TEXTSTRING
#if !defined(TQT_NO_SPRINTF)
    TQString toString( TQt::DateFormat f = TQt::TextDate ) const;
#endif
    TQString toString( const TQString& format ) const;
#endif
    bool   setYMD( int y, int m, int d );

    TQDate  addDays( int days )		const;
    TQDate  addMonths( int months )      const;
    TQDate  addYears( int years )        const;
    int	   daysTo( const TQDate & )	const;

    bool   operator==( const TQDate &d ) const { return jd == d.jd; }
    bool   operator!=( const TQDate &d ) const { return jd != d.jd; }
    bool   operator<( const TQDate &d )	const { return jd < d.jd; }
    bool   operator<=( const TQDate &d ) const { return jd <= d.jd; }
    bool   operator>( const TQDate &d )	const { return jd > d.jd; }
    bool   operator>=( const TQDate &d ) const { return jd >= d.jd; }

    static TQDate currentDate();
    static TQDate currentDate( TQt::TimeSpec );
#ifndef TQT_NO_DATESTRING
    static TQDate fromString( const TQString& s, TQt::DateFormat f = TQt::TextDate );
#endif
    static bool	 isValid( int y, int m, int d );
    static bool	 leapYear( int year );

    static uint	 gregorianToJulian( int y, int m, int d );
    static void	 julianToGregorian( uint jd, int &y, int &m, int &d );
private:
    uint	 jd;
    friend class TQDateTime;
#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQDate & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQDate & );
#endif
};


/*****************************************************************************
  TQTime class
 *****************************************************************************/

class TQ_EXPORT TQTime
{
public:
    TQTime() { ds=0; }				// set null time
    TQTime( int h, int m, int s=0, int ms=0 );	// set time

    bool   isNull()	 const { return ds == 0; }
    bool   isValid()	 const;			// valid time

    int	   hour()	 const;			// 0..23
    int	   minute()	 const;			// 0..59
    int	   second()	 const;			// 0..59
    int	   msec()	 const;			// 0..999
#ifndef TQT_NO_DATESTRING
#ifndef TQT_NO_SPRINTF
    TQString toString( TQt::DateFormat f = TQt::TextDate )	 const;
#endif
    TQString toString( const TQString& format ) const;
#endif
    bool   setHMS( int h, int m, int s, int ms=0 );

    TQTime  addSecs( int secs )		const;
    int	   secsTo( const TQTime & )	const;
    TQTime  addMSecs( int ms )		const;
    int	   msecsTo( const TQTime & )	const;

    bool   operator==( const TQTime &d ) const { return ds == d.ds; }
    bool   operator!=( const TQTime &d ) const { return ds != d.ds; }
    bool   operator<( const TQTime &d )	const { return ds < d.ds; }
    bool   operator<=( const TQTime &d ) const { return ds <= d.ds; }
    bool   operator>( const TQTime &d )	const { return ds > d.ds; }
    bool   operator>=( const TQTime &d ) const { return ds >= d.ds; }

    static TQTime currentTime();
    static TQTime currentTime( TQt::TimeSpec );
#ifndef TQT_NO_DATESTRING
    static TQTime fromString( const TQString& s, TQt::DateFormat f = TQt::TextDate );
#endif
    static bool	 isValid( int h, int m, int s, int ms=0 );

    void   start();
    int	   restart();
    int	   elapsed() const;

private:
    static bool currentTime( TQTime * );
    static bool currentTime( TQTime *, TQt::TimeSpec );

    uint   ds;
    friend class TQDateTime;
#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQTime & );
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQTime & );
#endif
};


/*****************************************************************************
  TQDateTime class
 *****************************************************************************/

class TQ_EXPORT TQDateTime
{
public:
    TQDateTime() {}				// set null date and null time
    TQDateTime( const TQDate & );
    TQDateTime( const TQDate &, const TQTime & );

    bool   isNull()	const		{ return d.isNull() && t.isNull(); }
    bool   isValid()	const		{ return d.isValid() && t.isValid(); }

    TQDate  date()	const		{ return d; }
    TQTime  time()	const		{ return t; }
    time_t  toTime_t()	const;
    void   setDate( const TQDate &date ) { d = date; }
    void   setTime( const TQTime &time ) { t = time; }
    void   setTime_t( time_t secsSince1Jan1970UTC );
    void   setTime_t( time_t secsSince1Jan1970UTC, TQt::TimeSpec );
#ifndef TQT_NO_DATESTRING
#ifndef TQT_NO_SPRINTF
    TQString toString( TQt::DateFormat f = TQt::TextDate )	const;
#endif
    TQString toString( const TQString& format ) const;
#endif
    TQDateTime addDays( int days )	const;
    TQDateTime addMonths( int months )   const;
    TQDateTime addYears( int years )     const;
    TQDateTime addSecs( int secs )	const;
    int	   daysTo( const TQDateTime & )	const;
    int	   secsTo( const TQDateTime & )	const;

    bool   operator==( const TQDateTime &dt ) const;
    bool   operator!=( const TQDateTime &dt ) const;
    bool   operator<( const TQDateTime &dt )  const;
    bool   operator<=( const TQDateTime &dt ) const;
    bool   operator>( const TQDateTime &dt )  const;
    bool   operator>=( const TQDateTime &dt ) const;

    static TQDateTime currentDateTime();
    static TQDateTime currentDateTime( TQt::TimeSpec );
#ifndef TQT_NO_DATESTRING
    static TQDateTime fromString( const TQString& s, TQt::DateFormat f = TQt::TextDate );
#endif
private:
    TQDate  d;
    TQTime  t;
#ifndef TQT_NO_DATASTREAM
    friend TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQDateTime &);
    friend TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQDateTime & );
#endif
};


/*****************************************************************************
  Date and time stream functions
 *****************************************************************************/

#ifndef TQT_NO_DATASTREAM
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQDate & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQDate & );
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQTime & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQTime & );
TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQDateTime & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQDateTime & );
#endif // TQT_NO_DATASTREAM

#endif // TQDATETIME_H

