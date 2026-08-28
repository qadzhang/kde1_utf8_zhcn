/****************************************************************************
**
** Definition of TQSettings related classes
**
** Created : 990124
**
** Copyright (C) 1999-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQSETTINGS_P_H
#define TQSETTINGS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of TQSettings. This header file may change from version to 
// version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef QT_H
#include "ntqstringlist.h"
#include "ntqmap.h"
#include "ntqvaluestack.h"
#endif // QT_H

#ifndef TQT_NO_SETTINGS
class TQSettingsSysPrivate;

// TQSettingsGroup is a map of key/value pairs
class TQSettingsGroup : public TQMap<TQString,TQString>
{
public:
    TQSettingsGroup();

    bool modified;
};

// TQSettingsHeading is a map of heading/group pairs
class TQSettingsHeading : public TQMap<TQString,TQSettingsGroup>
{
public:
    TQSettingsHeading::Iterator git;
    void read(const TQString &);
    void parseLine(TQTextStream &);
};


class TQSettingsPrivate
{
public:
    TQSettingsPrivate( TQSettings::Format format );
    ~TQSettingsPrivate();

    TQSettingsGroup readGroup();
    void removeGroup(const TQString &);
    void writeGroup(const TQString &, const TQString &);
    TQDateTime modificationTime();

    TQStringList searchPaths;
    TQMap<TQString,TQSettingsHeading> headings;
    TQString group;
    TQString heading;

    /*### static data brings threading trouble
    static TQString *defProduct;
    static TQString *defDomain;
    */
    TQValueStack<TQString> groupStack;
    TQString groupPrefix;

    bool groupDirty :1;
    bool modified :1;
    bool globalScope :1;

#if !defined(TQWS) && (defined(TQ_WS_WIN) || defined(Q_OS_MAC))
    // system dependent implementations to use the 
    // system specific setting database (ie. registry on Windows)

    TQSettingsSysPrivate *sysd;
    void	sysInit();
    void	sysClear();

    bool	sysWriteEntry( const TQString &, bool );
    bool	sysWriteEntry( const TQString &, double );
    bool	sysWriteEntry( const TQString &, int );
    bool	sysWriteEntry( const TQString &, const TQString & );
    bool	sysWriteEntry( const TQString &, const TQStringList & );
    bool	sysWriteEntry( const TQString &, const TQStringList &, const TQChar& sep );

    TQStringList sysEntryList(const TQString &) const;
    TQStringList sysSubkeyList(const TQString &) const;

    TQStringList sysReadListEntry( const TQString &, bool * = 0 ) const;
    TQStringList sysReadListEntry( const TQString &, const TQChar& sep, bool * = 0 ) const;
    TQString	sysReadEntry( const TQString &, const TQString &def = TQString::null, bool * = 0 ) const;
    int		sysReadNumEntry( const TQString &, int def = 0, bool * = 0 ) const;
    double	sysReadDoubleEntry( const TQString &, double def = 0, bool * = 0 ) const;
    bool	sysReadBoolEntry( const TQString &, bool def = 0, bool * = 0 ) const;

    bool	sysRemoveEntry( const TQString & );

    bool	sysSync();

    void	sysInsertSearchPath( TQSettings::System, const TQString & );
    void	sysRemoveSearchPath( TQSettings::System, const TQString & );
#endif

};
#endif //TQT_NO_SETTINGS
#endif // TQSETTINGS_P_H
