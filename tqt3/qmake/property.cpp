/****************************************************************************
**
** Implementation of TQMakeProperty class.
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

#include "property.h"
#include "option.h"
#include <ntqsettings.h>
#include <ntqdir.h>
#include <ntqmap.h>
#include <ntqstringlist.h>
#include <stdio.h>

TQStringList qmake_mkspec_paths(); //project.cpp

TQMakeProperty::TQMakeProperty() : sett(NULL)
{
}

TQMakeProperty::~TQMakeProperty()
{
    delete sett;;
    sett = NULL;
}


bool TQMakeProperty::initSettings()
{
    if(sett)
	return true;
    sett = new TQSettings;
    return true;
}

TQString
TQMakeProperty::keyBase(bool version) const
{
    TQString ret = "/TQMake/properties/";
    if(version)
	ret += TQString(qmake_version()) + "/";
    return ret;
}


TQString
TQMakeProperty::value(TQString v, bool just_check)
{
    if(v == "QT_INSTALL_PREFIX") {
#ifdef QT_INSTALL_PREFIX
	return QT_INSTALL_PREFIX;
#elif defined(HAVE_QCONFIG_CPP)
	return tqInstallPath();
#endif
    } else if(v == "QT_INSTALL_DATA") {
#ifdef QT_INSTALL_DATA
	return QT_INSTALL_DATA;
#elif defined(HAVE_QCONFIG_CPP)
	return tqInstallPathData();
#endif
    } else if(v == "QMAKE_MKSPECS") {
	return qmake_mkspec_paths().join(Option::target_mode == Option::TARG_WIN_MODE ? ";" : ":");
    } else if(v == "QMAKE_VERSION") {
	return qmake_version();
    }

    if(initSettings()) {
	bool ok;
	int slash = v.findRev('/');
	TQString ret = sett->readEntry(keyBase(slash == -1) + v, TQString::null, &ok);
	if(!ok) {
	    TQString version = qmake_version();
	    if(slash != -1) {
		version = v.left(slash-1);
		v = v.mid(slash+1);
	    }
	    TQStringList subs = sett->subkeyList(keyBase(false));
	    subs.sort();
	    for(TQStringList::Iterator it = subs.fromLast(); it != subs.end(); --it) {
		if((*it).isEmpty() || (*it) > version)
		    continue;
		ret = sett->readEntry(keyBase(false) + (*it) + "/" + v, TQString::null, &ok);
		if(ok) {
		    if(!just_check)
			debug_msg(1, "Fell back from %s -> %s for '%s'.", version.latin1(),
				  (*it).latin1(), v.latin1());
		    return ret;
		}
	    }
	}
	return ok ? ret : TQString::null;
    }
    return TQString::null;
}

bool
TQMakeProperty::hasValue(TQString v)
{
    if(initSettings())
	return !value(v, true).isNull();
    return false;
}

void
TQMakeProperty::setValue(TQString var, const TQString &val)
{
    if(initSettings())
	sett->writeEntry(keyBase() + var, val);
}

bool
TQMakeProperty::exec()
{
    bool ret = true;
    if(Option::qmake_mode == Option::QMAKE_QUERY_PROPERTY) {
	if(Option::prop::properties.isEmpty() && initSettings()) {
	    TQStringList subs = sett->subkeyList(keyBase(false));
	    subs.sort();
	    for(TQStringList::Iterator it = subs.fromLast(); it != subs.end(); --it) {
		if((*it).isEmpty())
		    continue;
		TQStringList keys = sett->entryList(keyBase(false) + (*it));
		for(TQStringList::Iterator it2 = keys.begin(); it2 != keys.end(); it2++) {
		    TQString ret = sett->readEntry(keyBase(false) + (*it) + "/" + (*it2));
		    if((*it) != qmake_version())
			fprintf(stdout, "%s/", (*it).latin1());
		    fprintf(stdout, "%s:%s\n", (*it2).latin1(), ret.latin1());
		}
	    }
	    return true;
	}
	for(TQStringList::Iterator it = Option::prop::properties.begin(); 
	    it != Option::prop::properties.end(); it++) {
	    if(Option::prop::properties.count() > 1)
		fprintf(stdout, "%s:", (*it).latin1());
	    if(!hasValue((*it))) {
		ret = false;
		fprintf(stdout, "**Unknown**\n");
	    } else {
		fprintf(stdout, "%s\n", value((*it)).latin1());
	    }
	}
    } else if(Option::qmake_mode == Option::QMAKE_SET_PROPERTY) {
	for(TQStringList::Iterator it = Option::prop::properties.begin(); 
	    it != Option::prop::properties.end(); it++) {
	    TQString var = (*it);
	    it++;
	    if(it == Option::prop::properties.end()) {
		ret = false;
		break;
	    }
	    if(!var.startsWith("."))
		setValue(var, (*it));
	}
    }
    return ret;
}
