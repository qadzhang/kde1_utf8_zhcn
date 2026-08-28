/****************************************************************************
**
** Definition of TQBig5Codec class
**
** Created : 990713
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

// Most of the code here was originally written by Ming-Che Chuang and
// is included in TQt with the author's permission, and the grateful
// thanks of the Trolltech team.

#ifndef TQBIG5CODEC_H
#define TQBIG5CODEC_H

#ifndef QT_H
#include "ntqtextcodec.h"
#endif // QT_H

#ifndef TQT_NO_BIG_CODECS

class TQBig5Codec : public TQTextCodec {
public:
    virtual int mibEnum() const;
    const char* name() const;

    TQTextDecoder* makeDecoder() const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut) const;
    TQString toUnicode(const char* chars, int len) const;

    int heuristicContentMatch(const char* chars, int len) const;
    int heuristicNameMatch(const char* hint) const;
};

class TQBig5hkscsCodec : public TQTextCodec {
public:
    virtual int mibEnum() const;
    const char* name() const;

    TQTextDecoder* makeDecoder() const;

#if !defined(Q_NO_USING_KEYWORD)
    using TQTextCodec::fromUnicode;
#endif
    TQCString fromUnicode(const TQString& uc, int& lenInOut) const;
    TQString toUnicode(const char* chars, int len) const;

    int heuristicContentMatch(const char* chars, int len) const;
    int heuristicNameMatch(const char* hint) const;
};

#endif
#endif
