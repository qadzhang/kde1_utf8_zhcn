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

#ifndef UIC_H
#define UIC_H
#include <ntqdom.h>
#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqmap.h>
#include <ntqtextstream.h>
#include <ntqpalette.h>
#include <ntqvariant.h>

class Uic : public TQt
{
public:
    Uic( const TQString &fn, const char *outputFn, TQTextStream& out,
	 TQDomDocument doc, bool decl, bool subcl, const TQString &trm,
	 const TQString& subclname, bool omitForwardDecls );

    void createFormDecl( const TQDomElement &e );
    void createFormImpl( const TQDomElement &e );

    void createSubDecl( const TQDomElement &e, const TQString& subclname );
    void createSubImpl( const TQDomElement &e, const TQString& subclname );

    void createObjectDecl( const TQDomElement &e );
    void createSpacerDecl( const TQDomElement &e );
    void createActionDecl( const TQDomElement &e );
    void createToolbarDecl( const TQDomElement &e );
    void createMenuBarDecl( const TQDomElement &e );
    void createPopupMenuDecl( const TQDomElement &e );  
    void createActionImpl( const TQDomElement &e, const TQString &parent );
    void createToolbarImpl( const TQDomElement &e, const TQString &parentClass, const TQString &parent );
    void createMenuBarImpl( const TQDomElement &e, const TQString &parentClass, const TQString &parent );
    void createPopupMenuImpl( const TQDomElement &e, const TQString &parentClass, const TQString &parent );
    TQString createObjectImpl( const TQDomElement &e, const TQString& parentClass, const TQString& parent, const TQString& layout = TQString::null );
    TQString createLayoutImpl( const TQDomElement &e, const TQString& parentClass, const TQString& parent, const TQString& layout = TQString::null );
    TQString createObjectInstance( const TQString& objClass, const TQString& parent, const TQString& objName );
    TQString createSpacerImpl( const TQDomElement &e, const TQString& parentClass, const TQString& parent, const TQString& layout = TQString::null );
    void createExclusiveProperty( const TQDomElement & e, const TQString& exclusiveProp );
    TQString createListBoxItemImpl( const TQDomElement &e, const TQString &parent, TQString *value = 0 );
    TQString createIconViewItemImpl( const TQDomElement &e, const TQString &parent );
    TQString createListViewColumnImpl( const TQDomElement &e, const TQString &parent, TQString *value = 0 );
    TQString createTableRowColumnImpl( const TQDomElement &e, const TQString &parent, TQString *value = 0 );
    TQString createListViewItemImpl( const TQDomElement &e, const TQString &parent,
				    const TQString &parentItem );
    void createColorGroupImpl( const TQString& cg, const TQDomElement& e );
    TQColorGroup loadColorGroup( const TQDomElement &e );

    TQDomElement getObjectProperty( const TQDomElement& e, const TQString& name );
    TQString getPixmapLoaderFunction( const TQDomElement& e );
    TQString getFormClassName( const TQDomElement& e );
    TQString getClassName( const TQDomElement& e );
    TQString getObjectName( const TQDomElement& e );
    TQString getLayoutName( const TQDomElement& e );
    TQString getInclude( const TQString& className );

    TQString setObjectProperty( const TQString& objClass, const TQString& obj, const TQString &prop, const TQDomElement &e, bool stdset );

    TQString registerObject( const TQString& name );
    TQString registeredName( const TQString& name );
    bool isObjectRegistered( const TQString& name );
    TQStringList unique( const TQStringList& );

    TQString trcall( const TQString& sourceText, const TQString& comment = "" );

    static void embed( TQTextStream& out, const char* project, const TQStringList& images );

private:
    void registerLayouts ( const TQDomElement& e );

    TQTextStream& out;
    TQTextOStream trout;
    TQString languageChangeBody;
    TQCString outputFileName;
    TQStringList objectNames;
    TQMap<TQString,TQString> objectMapper;
    TQString indent;
    TQStringList tags;
    TQStringList layouts;
    TQString formName;
    TQString lastItem;
    TQString trmacro;
    bool nofwd;

    struct Buddy
    {
	Buddy( const TQString& k, const TQString& b )
	    : key( k ), buddy( b ) {}
	Buddy(){} // for valuelist
	TQString key;
	TQString buddy;
	bool operator==( const Buddy& other ) const
	    { return (key == other.key); }
    };
    struct CustomInclude
    {
	TQString header;
	TQString location;
    };
    TQValueList<Buddy> buddies;

    TQStringList layoutObjects;
    bool isLayout( const TQString& name ) const;

    uint item_used : 1;
    uint cg_used : 1;
    uint pal_used : 1;
    uint stdsetdef : 1;
    uint externPixmaps : 1;

    TQString uiFileVersion;
    TQString nameOfClass;
    TQStringList namespaces;
    TQString bareNameOfClass;
    TQString pixmapLoaderFunction;

    void registerDatabases( const TQDomElement& e );
    bool isWidgetInTable( const TQDomElement& e, const TQString& connection, const TQString& table );
    bool isFrameworkCodeGenerated( const TQDomElement& e );
    TQString getDatabaseInfo( const TQDomElement& e, const TQString& tag );
    void createFormImpl( const TQDomElement& e, const TQString& form, const TQString& connection, const TQString& table );
    void writeFunctionsDecl( const TQStringList &fuLst, const TQStringList &typLst, const TQStringList &specLst );
    void writeFunctionsSubDecl( const TQStringList &fuLst, const TQStringList &typLst, const TQStringList &specLst );
    void writeFunctionsSubImpl( const TQStringList &fuLst, const TQStringList &typLst, const TQStringList &specLst,
				const TQString &subClass, const TQString &descr );
    TQStringList dbConnections;
    TQMap< TQString, TQStringList > dbCursors;
    TQMap< TQString, TQStringList > dbForms;

    static bool isMainWindow;
    static TQString mkBool( bool b );
    static TQString mkBool( const TQString& s );
    bool toBool( const TQString& s );
    static TQString fixString( const TQString &str, bool encode = false );
    static bool onlyAscii;
    static TQString mkStdSet( const TQString& prop );
    static TQString getComment( const TQDomNode& n );
    TQVariant defSpacing, defMargin;
    TQString fileName;
    bool writeFunctImpl;
};

#endif
