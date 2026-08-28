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

#include "languageinterfaceimpl.h"
#include <ntqobject.h>
#include <designerinterface.h>
#include <ntqfile.h>
#include "yyreg.h"
#include <ntqmetaobject.h>

LanguageInterfaceImpl::LanguageInterfaceImpl( TQUnknownInterface *outer )
    : parent( outer ), ref( 0 )
{
}

ulong LanguageInterfaceImpl::addRef()
{
    return parent ? parent->addRef() : ref++;
}

ulong LanguageInterfaceImpl::release()
{
    if ( parent )
	return parent->release();
    if ( !--ref ) {
	delete this;
	return 0;
    }
    return ref;
}

TQRESULT LanguageInterfaceImpl::queryInterface( const TQUuid &uuid, TQUnknownInterface** iface )
{
    if ( parent )
	return parent->queryInterface( uuid, iface );

    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = (TQUnknownInterface*)this;
    else if ( uuid == IID_Language )
	*iface = (LanguageInterface*)this;
    else
	return TQE_NOINTERFACE;

    (*iface)->addRef();
    return TQS_OK;
}


class NormalizeObject : public TQObject
{
public:
    NormalizeObject() : TQObject() {}
    static TQCString normalizeSignalSlot( const char *signalSlot ) { return TQObject::normalizeSignalSlot( signalSlot ); }
};

void LanguageInterfaceImpl::functions( const TQString &code, TQValueList<Function> *functionMap ) const
{
    TQValueList<CppFunction> l;
    extractCppFunctions( code, &l );
    for ( TQValueList<CppFunction>::Iterator it = l.begin(); it != l.end(); ++it ) {
	Function func;
	func.name = (*it).prototype();
	func.name.remove( 0, (*it).returnType().length() );
	if ( func.name.find( "::" ) == -1 )
	    continue;
	func.name.remove( (uint)0, func.name.find( "::" ) + 2 );
	func.body = (*it).body();
	func.returnType = (*it).returnType();
	func.start = (*it).functionStartLineNum();
	func.end = (*it).closingBraceLineNum();
	functionMap->append( func );
    }
}

TQString LanguageInterfaceImpl::createFunctionStart( const TQString &className, const TQString &func,
						    const TQString &returnType,
						    const TQString & )
{
    return returnType + " " + className + "::" + func;
}

TQStringList LanguageInterfaceImpl::definitions() const
{
    TQStringList lst;
    lst << "Includes (in Implementation)" << "Includes (in Declaration)" << "Forward Declarations" << "Signals";
    return lst;
}

TQStringList LanguageInterfaceImpl::definitionEntries( const TQString &definition, TQUnknownInterface *designerIface ) const
{
    DesignerInterface *iface = 0;
    designerIface->queryInterface( IID_Designer, (TQUnknownInterface**) &iface );
    if ( !iface )
	return TQStringList();
    DesignerFormWindow *fw = iface->currentForm();
    if ( !fw )
	return TQStringList();
    TQStringList lst;
    if ( definition == "Includes (in Implementation)" ) {
	lst = fw->implementationIncludes();
    } else if ( definition == "Includes (in Declaration)" ) {
	lst = fw->declarationIncludes();
    } else if ( definition == "Forward Declarations" ) {
	lst = fw->forwardDeclarations();
    } else if ( definition == "Signals" ) {
	lst = fw->signalList();
    }
    iface->release();
    return lst;
}

void LanguageInterfaceImpl::setDefinitionEntries( const TQString &definition, const TQStringList &entries, TQUnknownInterface *designerIface )
{
    DesignerInterface *iface = 0;
    designerIface->queryInterface( IID_Designer, (TQUnknownInterface**) &iface );
    if ( !iface )
	return;
    DesignerFormWindow *fw = iface->currentForm();
    if ( !fw )
	return;
    if ( definition == "Includes (in Implementation)" ) {
	fw->setImplementationIncludes( entries );
    } else if ( definition == "Includes (in Declaration)" ) {
	fw->setDeclarationIncludes( entries );
    } else if ( definition == "Forward Declarations" ) {
	fw->setForwardDeclarations( entries );
    } else if ( definition == "Signals" ) {
	fw->setSignalList( entries );
    }
    iface->release();
}

TQString LanguageInterfaceImpl::createEmptyFunction()
{
    return "{\n\n}\n";
}

bool LanguageInterfaceImpl::supports( Support s ) const
{
    if ( s == ReturnType )
	return true;
    if ( s == ConnectionsToCustomSlots )
	return true;
    return false;
}

TQStringList LanguageInterfaceImpl::fileFilterList() const
{
    TQStringList f;
    f << "C++ Files (*.cpp *.C *.cxx *.c++ *.c *.h *.H *.hpp *.hxx)";
    return f;

}
TQStringList LanguageInterfaceImpl::fileExtensionList() const
{
    TQStringList f;
    f << "cpp" << "C" << "cxx" << "c++" << "c" <<"h" << "H" << "hpp" << "hxx";
    return f;
}

TQString LanguageInterfaceImpl::projectKeyForExtension( const TQString &extension ) const
{
    // also handle something like foo.ut.cpp
    TQString ext = extension;
    int i = ext.findRev('.');
    if ( i > -1 && i < (int)(ext.length()-1) )
        ext = ext.mid( i + 1 );
    if ( ext[ 0 ] == 'c' || ext[ 0 ] == 'C' )
	    return "SOURCES";
    return "HEADERS";
}

void LanguageInterfaceImpl::sourceProjectKeys( TQStringList &keys ) const
{
    keys << "HEADERS" << "SOURCES";
}

 class CheckObject : public TQObject
{
public:
    CheckObject() {}
    bool checkConnectArgs( const char *signal, const char *member ) { return TQObject::checkConnectArgs( signal, 0, member ); }

};

bool LanguageInterfaceImpl::canConnect( const TQString &signal, const TQString &slot )
{
    CheckObject o;
    return o.checkConnectArgs( signal.latin1(), slot.latin1() );
}

void LanguageInterfaceImpl::loadFormCode( const TQString &, const TQString &filename,
					       TQValueList<Function> &functions,
					       TQStringList &,
					       TQValueList<Connection> & )
{
    TQFile f( filename );
    if ( !f.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &f );
    TQString code( ts.read() );
    this->functions( code, &functions );
}

void LanguageInterfaceImpl::preferedExtensions( TQMap<TQString, TQString> &extensionMap ) const
{
    extensionMap.insert( "cpp", "C++ Source File" );
    extensionMap.insert( "h", "C++ Header File" );
}

TQStrList LanguageInterfaceImpl::signalNames( TQObject *obj ) const
{
    TQStrList sigs;
    sigs = obj->metaObject()->signalNames( true );
    sigs.remove( "destroyed()" );
    return sigs;
}
