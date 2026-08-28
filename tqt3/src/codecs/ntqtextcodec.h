/****************************************************************************
**
** Definition of TQTextCodec class
**
** Created : 981015
**
** Copyright (C) 1998-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQTEXTCODEC_H
#define TQTEXTCODEC_H

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#ifndef TQT_NO_TEXTCODEC

class TQTextCodec;
class TQIODevice;
class TQFont;

class TQ_EXPORT TQTextEncoder {
public:
    virtual ~TQTextEncoder();
    virtual TQCString fromUnicode(const TQString& uc, int& lenInOut) = 0;
};

class TQ_EXPORT TQTextDecoder {
public:
    virtual ~TQTextDecoder();
    virtual TQString toUnicode(const char* chars, int len) = 0;
};

class TQ_EXPORT TQTextCodec {
public:
    virtual ~TQTextCodec();

#ifndef TQT_NO_CODECS
    static TQTextCodec* loadCharmap(TQIODevice*);
    static TQTextCodec* loadCharmapFile(TQString filename);
#endif //TQT_NO_CODECS
    static TQTextCodec* codecForMib(int mib);
    static TQTextCodec* codecForName(const char* hint, int accuracy=0);
    static TQTextCodec* codecForContent(const char* chars, int len);
    static TQTextCodec* codecForIndex(int i);
    static TQTextCodec* codecForLocale();
    static void setCodecForLocale(TQTextCodec *c);

    static TQTextCodec* codecForTr();
    static void setCodecForTr(TQTextCodec *c);
    static TQTextCodec* codecForCStrings();
    static void setCodecForCStrings(TQTextCodec *c);

    static void deleteAllCodecs();

    static const char* locale();

    virtual const char* name() const = 0;
    virtual const char* mimeName() const;
    virtual int mibEnum() const = 0;

    virtual TQTextDecoder* makeDecoder() const;
    virtual TQTextEncoder* makeEncoder() const;

    virtual TQString toUnicode(const char* chars, int len) const;
    virtual TQCString fromUnicode(const TQString& uc, int& lenInOut) const;

    TQCString fromUnicode(const TQString& uc) const;
    TQString toUnicode(const TQByteArray&, int len) const;
    TQString toUnicode(const TQByteArray&) const;
    TQString toUnicode(const TQCString&, int len) const;
    TQString toUnicode(const TQCString&) const;
    TQString toUnicode(const char* chars) const;
    virtual bool canEncode( TQChar ) const;
    virtual bool canEncode( const TQString& ) const;

    virtual int heuristicContentMatch(const char* chars, int len) const = 0;
    virtual int heuristicNameMatch(const char* hint) const;

    virtual TQByteArray fromUnicode(const TQString& uc, int from, int len) const;
    virtual unsigned short characterFromUnicode(const TQString &str, int pos) const;

protected:
    TQTextCodec();
    static int simpleHeuristicNameMatch(const char* name, const char* hint);

private:
    friend class TQFont;
    friend class TQFontEngineXLFD;
    void fromUnicodeInternal( const TQChar *in, unsigned short *out,  int length );

    static TQTextCodec *cftr;
    static TQTextCodec *cfcs;
};

inline TQTextCodec* TQTextCodec::codecForTr() { return cftr; }
inline void TQTextCodec::setCodecForTr(TQTextCodec *c) { cftr = c; }
inline TQTextCodec* TQTextCodec::codecForCStrings() { return cfcs; }
inline void TQTextCodec::setCodecForCStrings(TQTextCodec *c) { cfcs = c; }

#endif // TQT_NO_TEXTCODEC
#endif // TQTEXTCODEC_H
