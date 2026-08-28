/****************************************************************************
**
** Definition of TQMakeProject class.
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

#ifndef __PROJECT_H__
#define __PROJECT_H__

#include <ntqstringlist.h>
#include <ntqstring.h>
#include <ntqmap.h>

class TQMakeProperty;

class TQMakeProject
{
    enum TestStatus { TestNone, TestFound, TestSeek } test_status;
    int scope_block, scope_flag;

    TQString pfile, cfile;
    TQMakeProperty *prop;
    void reset();
    TQMap<TQString, TQStringList> vars, base_vars, cache;
    bool parse(const TQString &text, TQMap<TQString, TQStringList> &place);
    bool doProjectTest(const TQString &func, const TQString &params, TQMap<TQString, TQStringList> &place);
    bool doProjectTest(const TQString &func, TQStringList args, TQMap<TQString, TQStringList> &place);
    bool doProjectCheckReqs(const TQStringList &deps, TQMap<TQString, TQStringList> &place);
    TQString doVariableReplace(TQString &str, const TQMap<TQString, TQStringList> &place);

public:
    TQMakeProject();
    TQMakeProject(TQMakeProperty *);

    enum { ReadCache=0x01, ReadConf=0x02, ReadCmdLine=0x04, ReadProFile=0x08, ReadPostFiles=0x10, ReadAll=0xFF };
    bool read(const TQString &project, const TQString &pwd, uchar cmd=ReadAll);
    bool read(uchar cmd=ReadAll);

    TQString projectFile();
    TQString configFile();

    bool isEmpty(const TQString &v);
    TQStringList &values(const TQString &v);
    TQString first(const TQString &v);
    TQMap<TQString, TQStringList> &variables();
    bool isActiveConfig(const TQString &x, bool regex=false, TQMap<TQString, TQStringList> *place=NULL);

protected:
    friend class MakefileGenerator;
    bool read(const TQString &file, TQMap<TQString, TQStringList> &place);

};

inline TQString TQMakeProject::projectFile()
{
#if defined(Q_CC_SUN) && (__SUNPRO_CC == 0x500) || defined(Q_CC_HP)
    // workaround for Sun WorkShop 5.0 bug fixed in Forte 6
    if (pfile == "-")
	return TQString("(stdin)");
    else
	return pfile;
#else
    return pfile == "-" ? TQString("(stdin)") : pfile;
#endif
}

inline TQString TQMakeProject::configFile()
{ return cfile; }

inline bool TQMakeProject::isEmpty(const TQString &v)
{ return !vars.contains(v) || vars[v].isEmpty(); }

inline TQStringList &TQMakeProject::values(const TQString &v)
{ return vars[v]; }

inline TQString TQMakeProject::first(const TQString &v)
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

inline TQMap<TQString, TQStringList> &TQMakeProject::variables()
{ return vars; }

#endif /* __PROJECT_H__ */
