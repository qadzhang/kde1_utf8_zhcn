/****************************************************************************
**
** Font utilities for X11
**
** Created : 20001101
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

#ifndef TQFONTCODECS_P_H
#define TQFONTCODECS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of qfontencodings_x11.cpp and qfont_x11.cpp.  This header file may
// change from version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqglobal.h"
#include "ntqtextcodec.h"
#endif // QT_H


#ifndef TQT_NO_CODECS
#ifndef TQT_NO_BIG_CODECS


class TQJpUnicodeConv;


class TQFontJis0201Codec : public TQTextCodec
{
public:
    TQFontJis0201Codec();

    const char *name() const;

    int mibEnum() const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
    int heuristicNameMatch(const char* hint) const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};


class TQFontJis0208Codec : public TQTextCodec
{
public:
    TQFontJis0208Codec();
    ~TQFontJis0208Codec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
    int heuristicNameMatch(const char* hint) const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;

private:
    TQJpUnicodeConv *convJP;
};




class TQFontKsc5601Codec : public TQTextCodec
{
public:
    TQFontKsc5601Codec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};




class TQFontGb2312Codec : public TQTextCodec
{
public:
    TQFontGb2312Codec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};




class TQFontGbkCodec : public TQTextCodec
{
public:
    TQFontGbkCodec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
    int heuristicNameMatch(const char* hint) const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};




class TQFontGb18030_0Codec : public TQTextCodec
{
public:
    TQFontGb18030_0Codec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};




class TQFontBig5Codec : public TQTextCodec
{
public:
    TQFontBig5Codec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    int heuristicNameMatch(const char* hint) const;
    bool canEncode( TQChar ) const;
};



class TQFontBig5hkscsCodec : public TQTextCodec
{
public:
    TQFontBig5hkscsCodec();

    // Return the official name for the encoding.
    const char* name() const ;

    // Return the MIB enum for the encoding if it is listed in the
    // IANA character-sets encoding file.
    int mibEnum() const ;

    // Converts len characters from chars to Unicode.
    TQString toUnicode(const char* chars, int len) const ;

    // Converts lenInOut characters (of type TQChar) from the start of
    // the string uc, returning a TQCString result, and also returning
    // the length of the result in lenInOut.
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;
    int heuristicNameMatch(const char* hint) const;
#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};


class TQFontLaoCodec : public TQTextCodec
{
public:
    TQFontLaoCodec();

    const char *name() const;

    int mibEnum() const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut ) const;
    void fromUnicode( const TQChar *in, unsigned short *out, int length ) const;

    unsigned short characterFromUnicode(const TQString &str, int pos) const;

    int heuristicContentMatch(const char *, int) const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::canEncode;
#endif
    bool canEncode( TQChar ) const;
};

#endif // TQT_NO_BIG_CODECS
#endif // TQT_NO_CODECS

#endif // TQFONTCODECS_P_H
