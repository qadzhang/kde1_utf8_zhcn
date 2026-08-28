/****************************************************************************
**
** Definition of TQSettings class
**
** Created : 000626
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQSETTINGS_H
#define TQSETTINGS_H

#ifndef QT_H
#include "ntqdatetime.h"
#include "ntqstringlist.h"
#endif // QT_H

#ifndef TQT_NO_SETTINGS

class TQSettingsPrivate;


class TQ_EXPORT TQSettings
{
public:
    enum Format {
	Native = 0,
	Ini
    };
    enum System {
	Unix = 0,
	Windows,
	Mac
    };
    enum Scope {
	User,
	Global
    };

    TQSettings();
    TQSettings( Format format );

    ~TQSettings();

    bool	writeEntry( const TQString &, bool );
    bool	writeEntry( const TQString &, double );
    bool	writeEntry( const TQString &, int );
    bool	writeEntry( const TQString &, const char * );
    bool	writeEntry( const TQString &, const TQString & );
    bool	writeEntry( const TQString &, const TQStringList & );
    bool	writeEntry( const TQString &, const TQStringList &, const TQChar& sep );

    TQStringList entryList(const TQString &) const;
    TQStringList subkeyList(const TQString &) const;

    //### remove non const versions in 4.0
    TQStringList readListEntry( const TQString &, bool * = 0 );
    TQStringList readListEntry( const TQString &, const TQChar& sep, bool * = 0 );
    TQString	readEntry( const TQString &, const TQString &def = TQString::null, bool * = 0 );
    int		readNumEntry( const TQString &, int def = 0, bool * = 0 );
    double	readDoubleEntry( const TQString &, double def = 0, bool * = 0 );
    bool	readBoolEntry( const TQString &, bool def = false, bool * = 0 );

    //### make those non-inlined in 4.0
    TQStringList readListEntry( const TQString &key, bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readListEntry( key, ok );
    }
    TQStringList readListEntry( const TQString &key, const TQChar& sep, bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readListEntry( key, sep, ok );
    }
    TQString	readEntry( const TQString &key, const TQString &def = TQString::null,
			   bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readEntry( key, def, ok );
    }
    int		readNumEntry( const TQString &key, int def = 0, bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readNumEntry( key, def, ok );
    }

    double	readDoubleEntry( const TQString &key, double def = 0, bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readDoubleEntry( key, def, ok );
    }
    bool	readBoolEntry( const TQString &key, bool def = false, bool *ok = 0 ) const
    {
	TQSettings *that = (TQSettings*)this;
	return that->readBoolEntry( key, def, ok );
    }

    bool	removeEntry( const TQString & );

    void insertSearchPath( System, const TQString & );
    void removeSearchPath( System, const TQString & );

    void setPath( const TQString &domain, const TQString &product, Scope = Global );

    void beginGroup( const TQString &group );
    void endGroup();
    void resetGroup();
    TQString group() const;

    bool sync();

private:
    TQSettingsPrivate *d;

#if defined(TQ_DISABLE_COPY)
    TQSettings(const TQSettings &);
    TQSettings &operator=(const TQSettings &);
#endif

    TQDateTime lastModificationTime( const TQString & );

    friend class TQApplication;
};

#endif // TQT_NO_SETTINGS
#endif // TQSETTINGS_H
