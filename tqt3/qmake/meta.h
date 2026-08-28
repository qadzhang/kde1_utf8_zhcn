/****************************************************************************
**
** Definition of TQMakeMetaInfo class.
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of qmake.
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

#ifndef __META_H__
#define __META_H__

#include <ntqmap.h>
#include <ntqstringlist.h>
#include <ntqstring.h>

class TQMakeMetaInfo 
{
    bool readLibtoolFile(const TQString &f);
    bool readPkgCfgFile(const TQString &f);
    TQMap<TQString, TQStringList> vars;
    TQString meta_type;
    static TQMap<TQString, TQMap<TQString, TQStringList> > cache_vars;
    void clear();
public:
    TQMakeMetaInfo();

    bool readLib(const TQString &lib);
    static TQString findLib(const TQString &lib);
    static bool libExists(const TQString &lib);
    TQString type() const;

    bool isEmpty(const TQString &v);
    TQStringList &values(const TQString &v);
    TQString first(const TQString &v);
    TQMap<TQString, TQStringList> &variables();
};

inline bool TQMakeMetaInfo::isEmpty(const TQString &v)
{ return !vars.contains(v) || vars[v].isEmpty(); }

inline TQString TQMakeMetaInfo::type() const
{ return meta_type; }

inline TQStringList &TQMakeMetaInfo::values(const TQString &v)
{ return vars[v]; }

inline TQString TQMakeMetaInfo::first(const TQString &v)
{
#if defined(Q_CC_SUN) && (__SUNPRO_CC == 0x500) || defined(Q_CC_HP)
    // workaround for Sun WorkShop 5.0 bug fixed in Forte 6
    if (isEmpty(v))
	return TQString("");
    else
	return vars[v].first();
#else
    return isEmpty(v) ? TQString("") : vars[v].first();
#endif
}

inline TQMap<TQString, TQStringList> &TQMakeMetaInfo::variables()
{ return vars; }

inline bool TQMakeMetaInfo::libExists(const TQString &lib)
{ return !findLib(lib).isNull(); }

#endif /* __META_H__ */
