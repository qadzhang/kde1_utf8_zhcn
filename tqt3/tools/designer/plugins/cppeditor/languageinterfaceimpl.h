/**********************************************************************
**
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

#ifndef LANGUAGEINTERFACEIMPL_H
#define LANGUAGEINTERFACEIMPL_H

#include <languageinterface.h>

class LanguageInterfaceImpl : public LanguageInterface
{
public:
    LanguageInterfaceImpl( TQUnknownInterface *outer = 0 );

    ulong addRef();
    ulong release();

    TQRESULT queryInterface( const TQUuid&, TQUnknownInterface** );

    void functions( const TQString &code, TQValueList<Function> *funcs ) const;
    void connections( const TQString &, TQValueList<Connection> * ) const {};
    TQString createFunctionStart( const TQString &className, const TQString &func,
				 const TQString &returnType, const TQString &access );
    TQStringList definitions() const;
    TQStringList definitionEntries( const TQString &definition, TQUnknownInterface *designerIface ) const;
    void setDefinitionEntries( const TQString &definition, const TQStringList &entries, TQUnknownInterface *designerIface );
    TQString createArguments( const TQString & ) { return TQString::null; }
    TQString createEmptyFunction();
    bool supports( Support s ) const;
    TQStringList fileFilterList() const;
    TQStringList fileExtensionList() const;
    void preferedExtensions( TQMap<TQString, TQString> &extensionMap ) const;
    void sourceProjectKeys( TQStringList &keys ) const;
    TQString projectKeyForExtension( const TQString &extension ) const;
    TQString cleanSignature( const TQString &sig ) { return sig; } // #### implement me
    void loadFormCode( const TQString &, const TQString &,
		       TQValueList<Function> &,
		       TQStringList &,
		       TQValueList<Connection> & );
    TQString formCodeExtension() const { return ".h"; }
    bool canConnect( const TQString &signal, const TQString &slot );
    void compressProject( const TQString &, const TQString &, bool ) {}
    TQString uncompressProject( const TQString &, const TQString & ) { return TQString::null; }
    TQString aboutText() const { return ""; }

    void addConnection( const TQString &, const TQString &,
			const TQString &, const TQString &,
			TQString * ) {}
    void removeConnection( const TQString &, const TQString &,
			   const TQString &, const TQString &,
			   TQString * ) {}
    TQStrList signalNames( TQObject *obj ) const;

private:
    TQUnknownInterface *parent;
    ulong ref;

};

#endif
