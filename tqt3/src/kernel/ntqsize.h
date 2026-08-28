/****************************************************************************
**
** Definition of TQSize class
**
** Created : 931028
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQSIZE_H
#define TQSIZE_H

#ifndef QT_H
#include "ntqpoint.h" // ### change to ntqwindowdefs.h?
#endif // QT_H

class TQ_EXPORT TQSize
// ### Make TQSize inherit TQt in TQt 4.0
{
public:
    // ### Move this enum to ntqnamespace.h in TQt 4.0
    enum ScaleMode {
	ScaleFree,
	ScaleMin,
	ScaleMax
    };

    TQSize();
    TQSize( int w, int h );

    bool isNull() const;
    bool isEmpty() const;
    bool isValid() const;

    int width() const;
    int height() const;
    void setWidth( int w );
    void setHeight( int h );
    void transpose();

    void scale( int w, int h, ScaleMode mode );
    void scale( const TQSize &s, ScaleMode mode );

    TQSize expandedTo( const TQSize & ) const;
    TQSize boundedTo( const TQSize & ) const;

    TQCOORD &rwidth();
    TQCOORD &rheight();

    TQSize &operator+=( const TQSize & );
    TQSize &operator-=( const TQSize & );
    TQSize &operator*=( int c );
    TQSize &operator*=( double c );
    TQSize &operator/=( int c );
    TQSize &operator/=( double c );

    friend inline bool operator==( const TQSize &, const TQSize & );
    friend inline bool operator!=( const TQSize &, const TQSize & );
    friend inline const TQSize operator+( const TQSize &, const TQSize & );
    friend inline const TQSize operator-( const TQSize &, const TQSize & );
    friend inline const TQSize operator*( const TQSize &, int );
    friend inline const TQSize operator*( int, const TQSize & );
    friend inline const TQSize operator*( const TQSize &, double );
    friend inline const TQSize operator*( double, const TQSize & );
    friend inline const TQSize operator/( const TQSize &, int );
    friend inline const TQSize operator/( const TQSize &, double );

private:
    static void warningDivByZero();

    TQCOORD wd;
    TQCOORD ht;
};


/*****************************************************************************
  TQSize stream functions
 *****************************************************************************/

TQ_EXPORT TQDataStream &operator<<( TQDataStream &, const TQSize & );
TQ_EXPORT TQDataStream &operator>>( TQDataStream &, TQSize & );


/*****************************************************************************
  TQSize inline functions
 *****************************************************************************/

inline TQSize::TQSize()
{ wd = ht = -1; }

inline TQSize::TQSize( int w, int h )
{ wd=(TQCOORD)w; ht=(TQCOORD)h; }

inline bool TQSize::isNull() const
{ return wd==0 && ht==0; }

inline bool TQSize::isEmpty() const
{ return wd<1 || ht<1; }

inline bool TQSize::isValid() const
{ return wd>=0 && ht>=0; }

inline int TQSize::width() const
{ return wd; }

inline int TQSize::height() const
{ return ht; }

inline void TQSize::setWidth( int w )
{ wd=(TQCOORD)w; }

inline void TQSize::setHeight( int h )
{ ht=(TQCOORD)h; }

inline TQCOORD &TQSize::rwidth()
{ return wd; }

inline TQCOORD &TQSize::rheight()
{ return ht; }

inline TQSize &TQSize::operator+=( const TQSize &s )
{ wd+=s.wd; ht+=s.ht; return *this; }

inline TQSize &TQSize::operator-=( const TQSize &s )
{ wd-=s.wd; ht-=s.ht; return *this; }

inline TQSize &TQSize::operator*=( int c )
{ wd*=(TQCOORD)c; ht*=(TQCOORD)c; return *this; }

inline TQSize &TQSize::operator*=( double c )
{ wd=(TQCOORD)(wd*c); ht=(TQCOORD)(ht*c); return *this; }

inline bool operator==( const TQSize &s1, const TQSize &s2 )
{ return s1.wd == s2.wd && s1.ht == s2.ht; }

inline bool operator!=( const TQSize &s1, const TQSize &s2 )
{ return s1.wd != s2.wd || s1.ht != s2.ht; }

inline const TQSize operator+( const TQSize & s1, const TQSize & s2 )
{ return TQSize(s1.wd+s2.wd, s1.ht+s2.ht); }

inline const TQSize operator-( const TQSize &s1, const TQSize &s2 )
{ return TQSize(s1.wd-s2.wd, s1.ht-s2.ht); }

inline const TQSize operator*( const TQSize &s, int c )
{ return TQSize(s.wd*c, s.ht*c); }

inline const TQSize operator*( int c, const TQSize &s )
{  return TQSize(s.wd*c, s.ht*c); }

inline const TQSize operator*( const TQSize &s, double c )
{ return TQSize((TQCOORD)(s.wd*c), (TQCOORD)(s.ht*c)); }

inline const TQSize operator*( double c, const TQSize &s )
{ return TQSize((TQCOORD)(s.wd*c), (TQCOORD)(s.ht*c)); }

inline TQSize &TQSize::operator/=( int c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0 )
	warningDivByZero();
#endif
    wd/=(TQCOORD)c; ht/=(TQCOORD)c;
    return *this;
}

inline TQSize &TQSize::operator/=( double c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0.0 )
	warningDivByZero();
#endif
    wd=(TQCOORD)(wd/c); ht=(TQCOORD)(ht/c);
    return *this;
}

inline const TQSize operator/( const TQSize &s, int c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0 )
	TQSize::warningDivByZero();
#endif
    return TQSize(s.wd/c, s.ht/c);
}

inline const TQSize operator/( const TQSize &s, double c )
{
#if defined(QT_CHECK_MATH)
    if ( c == 0.0 )
	TQSize::warningDivByZero();
#endif
    return TQSize((TQCOORD)(s.wd/c), (TQCOORD)(s.ht/c));
}

inline TQSize TQSize::expandedTo( const TQSize & otherSize ) const
{
    return TQSize( TQMAX(wd,otherSize.wd), TQMAX(ht,otherSize.ht) );
}

inline TQSize TQSize::boundedTo( const TQSize & otherSize ) const
{
    return TQSize( TQMIN(wd,otherSize.wd), TQMIN(ht,otherSize.ht) );
}


#endif // TQSIZE_H
