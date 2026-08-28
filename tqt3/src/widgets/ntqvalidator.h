/****************************************************************************
**
** Definition of validator classes
**
** Created : 970610
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQVALIDATOR_H
#define TQVALIDATOR_H

#ifndef QT_H
#include "ntqobject.h"
#include "ntqstring.h" // char*->TQString conversion
#include "ntqregexp.h" // TQString->TQRegExp conversion
#endif // QT_H

#ifndef TQT_NO_VALIDATOR


class TQ_EXPORT TQValidator : public TQObject
{
    TQ_OBJECT
public:
    TQValidator( TQObject * parent, const char *name = 0 );
    ~TQValidator();

    enum State { Invalid, Intermediate, Valid=Intermediate, Acceptable };

    virtual State validate( TQString &, int & ) const = 0;
    virtual void fixup( TQString & ) const;

private:
#if defined(TQ_DISABLE_COPY)
    TQValidator( const TQValidator & );
    TQValidator& operator=( const TQValidator & );
#endif
};


class TQ_EXPORT TQIntValidator : public TQValidator
{
    TQ_OBJECT
    TQ_PROPERTY( int bottom READ bottom WRITE setBottom )
    TQ_PROPERTY( int top READ top WRITE setTop )

public:
    TQIntValidator( TQObject * parent, const char *name = 0 );
    TQIntValidator( int bottom, int top,
		   TQObject * parent, const char *name = 0 );
    ~TQIntValidator();

    TQValidator::State validate( TQString &, int & ) const;

    void setBottom( int );
    void setTop( int );
    virtual void setRange( int bottom, int top );

    int bottom() const { return b; }
    int top() const { return t; }

private:
#if defined(TQ_DISABLE_COPY)
    TQIntValidator( const TQIntValidator & );
    TQIntValidator& operator=( const TQIntValidator & );
#endif

    int b, t;
};

#ifndef TQT_NO_REGEXP

class TQ_EXPORT TQDoubleValidator : public TQValidator
{
    TQ_OBJECT
    TQ_PROPERTY( double bottom READ bottom WRITE setBottom )
    TQ_PROPERTY( double top READ top WRITE setTop )
    TQ_PROPERTY( int decimals READ decimals WRITE setDecimals )

public:
    TQDoubleValidator( TQObject * parent, const char *name = 0 );
    TQDoubleValidator( double bottom, double top, int decimals,
		      TQObject * parent, const char *name = 0 );
    ~TQDoubleValidator();

    TQValidator::State validate( TQString &, int & ) const;

    virtual void setRange( double bottom, double top, int decimals = 0 );
    void setBottom( double );
    void setTop( double );
    void setDecimals( int );

    double bottom() const { return b; }
    double top() const { return t; }
    int decimals() const { return d; }

private:
#if defined(TQ_DISABLE_COPY)
    TQDoubleValidator( const TQDoubleValidator & );
    TQDoubleValidator& operator=( const TQDoubleValidator & );
#endif

    double b, t;
    int d;
};


class TQ_EXPORT TQRegExpValidator : public TQValidator
{
    TQ_OBJECT
    // TQ_PROPERTY( TQRegExp regExp READ regExp WRITE setRegExp )

public:
    TQRegExpValidator( TQObject *parent, const char *name = 0 );
    TQRegExpValidator( const TQRegExp& rx, TQObject *parent,
		      const char *name = 0 );
    ~TQRegExpValidator();

    virtual TQValidator::State validate( TQString& input, int& pos ) const;

    void setRegExp( const TQRegExp& rx );
    const TQRegExp& regExp() const { return r; }

private:
#if defined(TQ_DISABLE_COPY)
    TQRegExpValidator( const TQRegExpValidator& );
    TQRegExpValidator& operator=( const TQRegExpValidator& );
#endif

    TQRegExp r;
};
#endif // TQT_NO_REGEXP


#endif // TQT_NO_VALIDATOR

#endif // TQVALIDATOR_H
