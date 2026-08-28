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

#ifndef DOMTOOL_H
#define DOMTOOL_H

#include <ntqvariant.h>
#include <ntqnamespace.h>

class TQDomElement;
class TQDomDocument;

class TQ_EXPORT DomTool : public TQt
{
public:
    static TQVariant readProperty( const TQDomElement& e, const TQString& name, const TQVariant& defValue );
    static TQVariant readProperty( const TQDomElement& e, const TQString& name, const TQVariant& defValue, TQString& comment );
    static bool hasProperty( const TQDomElement& e, const TQString& name );
    static TQStringList propertiesOfType( const TQDomElement& e, const TQString& type );
    static TQVariant elementToVariant( const TQDomElement& e, const TQVariant& defValue );
    static TQVariant elementToVariant( const TQDomElement& e, const TQVariant& defValue, TQString &comment );
    static TQVariant readAttribute( const TQDomElement& e, const TQString& name, const TQVariant& defValue );
    static TQVariant readAttribute( const TQDomElement& e, const TQString& name, const TQVariant& defValue, TQString& comment );
    static bool hasAttribute( const TQDomElement& e, const TQString& name );
    static TQColor readColor( const TQDomElement &e );
    static void fixDocument( TQDomDocument& );
};


#endif // DOMTOOL_H
