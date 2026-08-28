/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "defs.h"

int size_type_to_int( TQSizePolicy::SizeType t )
{
    if ( t == TQSizePolicy::Fixed )
	return 0;
    if ( t == TQSizePolicy::Minimum )
	return 1;
    if ( t == TQSizePolicy::Maximum )
	return 2;
    if ( t == TQSizePolicy::Preferred )
	return 3;
    if ( t == TQSizePolicy::MinimumExpanding )
	return 4;
    if ( t == TQSizePolicy::Expanding )
	return 5;
    if ( t == TQSizePolicy::Ignored )
	return 6;
    return 0;
}

TQString size_type_to_string( TQSizePolicy::SizeType t )
{
    if ( t == TQSizePolicy::Fixed )
	return "Fixed";
    if ( t == TQSizePolicy::Minimum )
	return "Minimum";
    if ( t == TQSizePolicy::Maximum )
	return "Maximum";
    if ( t == TQSizePolicy::Preferred )
	return "Preferred";
    if ( t == TQSizePolicy::MinimumExpanding )
	return "MinimumExpanding";
    if ( t == TQSizePolicy::Expanding )
	return "Expanding";
    if ( t == TQSizePolicy::Ignored )
	return "Ignored";
    return TQString();
}

TQSizePolicy::SizeType int_to_size_type( int i )
{	
    if ( i == 0 )
	return TQSizePolicy::Fixed;
    if ( i == 1 )
	return TQSizePolicy::Minimum;
    if ( i == 2 )
	return TQSizePolicy::Maximum;
    if ( i == 3 )
	return TQSizePolicy::Preferred;
    if ( i == 4 )
	return TQSizePolicy::MinimumExpanding;
    if ( i == 5 )
	return TQSizePolicy::Expanding;
    if ( i == 6 )
	return TQSizePolicy::Ignored;
    return TQSizePolicy::Preferred;
}
