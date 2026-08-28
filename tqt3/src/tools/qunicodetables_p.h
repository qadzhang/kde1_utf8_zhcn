/****************************************************************************
**
** ???
**
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQUNICODETABLES_P_H
#define TQUNICODETABLES_P_H

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

#ifndef QT_H
#include "ntqstring.h"
#endif // QT_H

#ifdef TQT_NO_UNICODETABLES
# include <ctype.h>
#endif

class TQUnicodeTables {
public:
    static const TQ_UINT8 unicode_info[];
#ifndef TQT_NO_UNICODETABLES
    static const TQ_UINT16 decomposition_map[];
    static const TQ_UINT16 decomposition_info[];
    static const TQ_UINT16 ligature_map[];
    static const TQ_UINT16 ligature_info[];
    static const TQ_UINT8 direction_info[];
    static const TQ_UINT8 combining_info[];
    static const TQ_UINT16 case_info[];
    static const TQ_INT8 decimal_info[];
    static const TQ_UINT16 symmetricPairs[];
    static const int symmetricPairsSize;
    static const TQ_UINT8 line_break_info[];
#else
    static const TQ_UINT8 latin1_line_break_info[];
#endif
    static const unsigned char otherScripts[];
    static const unsigned char indicScripts[];
    static const unsigned char scriptTable[];
    enum { SCRIPTS_INDIC = 0x7e };

    // see http://www.unicode.org/reports/tr14/tr14-13.html
    // we don't use the XX and AI properties and map them to AL instead.
    enum LineBreakClass {
	LineBreak_OP, LineBreak_CL, LineBreak_QU, LineBreak_GL, LineBreak_NS,
	LineBreak_EX, LineBreak_SY, LineBreak_IS, LineBreak_PR, LineBreak_PO,
	LineBreak_NU, LineBreak_AL, LineBreak_ID, LineBreak_IN, LineBreak_HY,
	LineBreak_BA, LineBreak_BB, LineBreak_B2, LineBreak_ZW, LineBreak_CM,
	LineBreak_SA, LineBreak_BK, LineBreak_CR, LineBreak_LF, LineBreak_SG,
	LineBreak_CB, LineBreak_SP
    };
};


inline TQChar::Category category( const TQChar &c )
{
#ifdef TQT_NO_UNICODETABLES
    if ( c.unicode() > 0xff ) return TQChar::Letter_Uppercase; //########
    return (TQChar::Category)TQUnicodeTables::unicode_info[c.unicode()];
#else
    int uc = ((int)TQUnicodeTables::unicode_info[c.row()]) << 8;
    uc += c.cell();
    return (TQChar::Category)TQUnicodeTables::unicode_info[uc];
#endif // TQT_NO_UNICODETABLES
}

inline TQChar lower( const TQChar &c )
{
#ifndef TQT_NO_UNICODETABLES
    int row = c.row();
    int cell = c.cell();
    int ci = TQUnicodeTables::case_info[row];
    int uc = ((int)TQUnicodeTables::unicode_info[c.row()]) << 8;
    uc += c.cell();
    if (TQUnicodeTables::unicode_info[uc] != TQChar::Letter_Uppercase || !ci)
	return c;
    TQ_UINT16 lower = TQUnicodeTables::case_info[(ci<<8)+cell];
    return lower ? TQChar(lower) : c;
#else
    if ( c.row() )
	return c;
    return TQChar( tolower((uchar) c.latin1()) );
#endif
}

inline TQChar upper( const TQChar &c )
{
#ifndef TQT_NO_UNICODETABLES
    int row = c.row();
    int cell = c.cell();
    int ci = TQUnicodeTables::case_info[row];
    int uc = ((int)TQUnicodeTables::unicode_info[c.row()]) << 8;
    uc += c.cell();
    if (TQUnicodeTables::unicode_info[uc] != TQChar::Letter_Lowercase || !ci)
	return c;
    TQ_UINT16 upper = TQUnicodeTables::case_info[(ci<<8)+cell];
    return upper ? TQChar(upper) : c;
#else
    if ( c.row() )
	return c;
    return TQChar( toupper((uchar) c.latin1()) );
#endif
}

inline TQChar::Direction direction( const TQChar &c )
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::direction_info[c.row()];
    return (TQChar::Direction) (TQUnicodeTables::direction_info[(pos<<8)+c.cell()] & 0x1f);
#else
    Q_UNUSED(c);
    return TQChar::DirL;
#endif
}

inline bool mirrored( const TQChar &c )
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::direction_info[c.row()];
    return TQUnicodeTables::direction_info[(pos<<8)+c.cell()] > 128;
#else
    Q_UNUSED(c);
    return false;
#endif
}


inline TQChar mirroredChar( const TQChar &ch )
{
#ifndef TQT_NO_UNICODETABLES
    if(!::mirrored( ch ))
	return ch;

    int i;
    int c = ch.unicode();
    for (i = 0; i < TQUnicodeTables::symmetricPairsSize; i ++) {
	if (TQUnicodeTables::symmetricPairs[i] == c)
	    return TQUnicodeTables::symmetricPairs[(i%2) ? (i-1) : (i+1)];
    }
#endif
    return ch;
}

inline TQChar::Joining joining( const TQChar &ch )
{
#ifndef TQT_NO_UNICODETABLES
    int pos = TQUnicodeTables::direction_info[ch.row()];
    return (TQChar::Joining) ((TQUnicodeTables::direction_info[(pos<<8)+ch.cell()] >> 5) &0x3);
#else
    Q_UNUSED(ch);
    return TQChar::OtherJoining;
#endif
}

inline bool isMark( const TQChar &ch )
{
    TQChar::Category c = ::category( ch );
    return c >= TQChar::Mark_NonSpacing && c <= TQChar::Mark_Enclosing;
}

inline unsigned char combiningClass( const TQChar &ch )
{
#ifndef TQT_NO_UNICODETABLES
    const int pos = TQUnicodeTables::combining_info[ch.row()];
    return TQUnicodeTables::combining_info[(pos<<8) + ch.cell()];
#else
    Q_UNUSED(ch);
    return 0;
#endif
}

inline bool isSpace( const TQChar &ch )
{
    if( ch.unicode() >= 9 && ch.unicode() <=13 ) return true;
    TQChar::Category c = ::category( ch );
    return c >= TQChar::Separator_Space && c <= TQChar::Separator_Paragraph;
}

inline int lineBreakClass( const TQChar &ch )
{
#ifdef TQT_NO_UNICODETABLES
    return ch.row() ? TQUnicodeTables::LineBreak_AL
	: TQUnicodeTables::latin1_line_break_info[ch.cell()];
#else
    int pos = ((int)TQUnicodeTables::line_break_info[ch.row()] << 8) + ch.cell();
    return TQUnicodeTables::line_break_info[pos];
#endif
}

inline int scriptForChar( ushort uc )
{
    unsigned char script = TQUnicodeTables::scriptTable[(uc>>8)];
    if ( script >= TQUnicodeTables::SCRIPTS_INDIC ) {
	if ( script == TQUnicodeTables::SCRIPTS_INDIC ) {
	    script = TQUnicodeTables::indicScripts[ (uc-0x0900)>>7 ];
	} else {
	    // 0x80 + SCRIPTS_xx
	    unsigned char index = script-0x80;
	    unsigned char cell = uc &0xff;
	    while( TQUnicodeTables::otherScripts[index++] < cell )
		index++;
	    script = TQUnicodeTables::otherScripts[index];
	}
    }
    return script;
}

#ifdef TQ_WS_X11
#define SCRIPT_FOR_CHAR( script, c ) 	\
do { 						\
    unsigned short _uc = (c).unicode(); 		\
    if ( _uc < 0x100 ) {				\
	script = TQFont::Latin;		\
    } else { 					\
        script = (TQFont::Script)scriptForChar( _uc ); 	\
    } 						\
} while( false )
#else
#define SCRIPT_FOR_CHAR( script, c ) \
    script = (TQFont::Script)scriptForChar( (c).unicode() )
#endif

#endif
