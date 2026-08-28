/****************************************************************************
**
** Declaration of the TQLocalePrivate class
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

#ifndef TQLOCALE_P_H
#define TQLOCALE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

#include <ntqstring.h>

struct TQLocalePrivate
{
public:
    const TQChar &decimal() const { return (TQChar&)m_decimal; }
    const TQChar &group() const { return (TQChar&)m_group; }
    const TQChar &list() const { return (TQChar&)m_list; }
    const TQChar &percent() const { return (TQChar&)m_percent; }
    const TQChar &zero() const { return (TQChar&)m_zero; }
    TQChar plus() const { return TQChar('+'); }
    const TQChar &minus() const { return (TQChar&)m_minus; }
    const TQChar &exponential() const { return (TQChar&)m_exponential; }
    TQString infinity() const;
    TQString nan() const;

    TQ_UINT32 languageId() const { return m_language_id; }
    TQ_UINT32 countryId() const { return m_country_id; }

    bool isDigit(TQChar d) const;

    enum GroupSeparatorMode {
    	FailOnGroupSeparators,
	ParseGroupSeparators
    };

    enum DoubleForm {
	DFExponent = 0,     	    	// %e or %E
	DFDecimal,  	    	    	// %f or %F
	DFSignificantDigits,	    	// %g or %G
	_DFMax = DFSignificantDigits
    };

    enum Flags {
	NoFlags	    	= 0,
	
	// These correspond to the options in a printf format string
	Alternate           = 0x01,
	ZeroPadded          = 0x02,
	LeftAdjusted        = 0x04,
	BlankBeforePositive = 0x08,
	AlwaysShowSign      = 0x10,
	ThousandsGroup      = 0x20,
	CapitalEorX	    = 0x40 // %x, %e, %f, %g vs. %X, %E, %F, %G
    };

    TQString doubleToString(double d,
			   int precision = -1,
			   DoubleForm form = DFSignificantDigits,
			   int width = -1,
			   unsigned flags = NoFlags) const;
    TQString longLongToString(TQ_LLONG l, int precision = -1,
			     int base = 10,
			     int width = -1,
			     unsigned flags = NoFlags) const;
    TQString unsLongLongToString(TQ_ULLONG l, int precision = -1,
				int base = 10,
				int width = -1,
				unsigned flags = NoFlags) const;
    double stringToDouble(TQString num, bool *ok, GroupSeparatorMode group_sep_mode) const;
    TQ_LLONG stringToLongLong(TQString num, int base, bool *ok, GroupSeparatorMode group_sep_mode) const;
    TQ_ULLONG stringToUnsLongLong(TQString num, int base, bool *ok, GroupSeparatorMode group_sep_mode) const;
    bool removeGroupSeparators(TQString &num_str) const;
    bool numberToCLocale(TQString &locale_num, GroupSeparatorMode group_sep_mode) const;

    TQ_UINT32 m_language_id, m_country_id;

    TQ_UINT16 m_decimal, m_group, m_list, m_percent,
	m_zero, m_minus, m_exponential;

    static const TQString m_infinity;
    static const TQString m_nan;
    static const TQChar m_plus;

    static const char *systemLocaleName();
};

#endif
