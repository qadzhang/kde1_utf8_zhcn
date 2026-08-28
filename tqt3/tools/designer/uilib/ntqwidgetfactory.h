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

#ifndef TQWIDGETFACTORY_H
#define TQWIDGETFACTORY_H

#ifndef QT_H
#include <ntqstring.h>
#include <ntqptrlist.h>
#include <ntqimage.h>
#include <ntqpixmap.h>
#include <ntqvaluelist.h>
#include <ntqmap.h>
#include <ntqaction.h>
#endif // QT_H

class TQDomDocument;
class TQDomElement;
class TQLayout;
class TQListView;
class TQListViewItem;
class TQMenuBar;
class TQTable;
class TQWidget;
class TQWidgetFactoryPrivate;
class UibStrTable;

class TQ_EXPORT TQWidgetFactory
{
public:
    TQWidgetFactory();
    virtual ~TQWidgetFactory();

    static TQWidget *create( const TQString &uiFile, TQObject *connector = 0, TQWidget *parent = 0, const char *name = 0 );
    static TQWidget *create( TQIODevice *dev, TQObject *connector = 0, TQWidget *parent = 0, const char *name = 0 );
    static void addWidgetFactory( TQWidgetFactory *factory );
    static void loadImages( const TQString &dir );

    virtual TQWidget *createWidget( const TQString &className, TQWidget *parent, const char *name ) const;
    static TQStringList widgets();
    static bool supportsWidget( const TQString &widget );

private:
    enum LayoutType { HBox, VBox, Grid, NoLayout };
    void loadImageCollection( const TQDomElement &e );
    void loadConnections( const TQDomElement &e, TQObject *connector );
    void loadTabOrder( const TQDomElement &e );
    TQWidget *createWidgetInternal( const TQDomElement &e, TQWidget *parent, TQLayout* layout, const TQString &classNameArg );
    TQLayout *createLayout( TQWidget *widget, TQLayout*  layout, LayoutType type, bool isTQLayoutWidget = false );
    LayoutType layoutType( TQLayout *l ) const;
    void setProperty( TQObject* widget, const TQString &prop, TQVariant value );
    void setProperty( TQObject* widget, const TQString &prop, const TQDomElement &e );
    void createSpacer( const TQDomElement &e, TQLayout *layout );
    TQImage loadFromCollection( const TQString &name );
    TQPixmap loadPixmap( const TQString &name );
    TQPixmap loadPixmap( const TQDomElement &e );
    TQColorGroup loadColorGroup( const TQDomElement &e );
    void createListViewColumn( TQListView *lv, const TQString& txt,
			       const TQPixmap& pix, bool clickable,
			       bool resizable );
#ifndef TQT_NO_TABLE
    void createTableColumnOrRow( TQTable *table, const TQString& txt,
				 const TQPixmap& pix, const TQString& field,
				 bool isRow );
#endif
    void createColumn( const TQDomElement &e, TQWidget *widget );
    void loadItem( const TQDomElement &e, TQPixmap &pix, TQString &txt, bool &hasPixmap );
    void createItem( const TQDomElement &e, TQWidget *widget, TQListViewItem *i = 0 );
    void loadChildAction( TQObject *parent, const TQDomElement &e );
    void loadActions( const TQDomElement &e );
    void loadToolBars( const TQDomElement &e );
    void loadMenuBar( const TQDomElement &e );
    void loadPopupMenu( TQPopupMenu *p, const TQDomElement &e );
    void loadFunctions( const TQDomElement &e );
    TQAction *findAction( const TQString &name );
    void loadExtraSource();
    TQString translate( const char *sourceText, const char *comment = "" );
    TQString translate( const TQString& sourceText, const TQString& comment = TQString::null );

    void unpackUInt16( TQDataStream& in, TQ_UINT16& n );
    void unpackUInt32( TQDataStream& in, TQ_UINT32& n );
    void unpackByteArray( TQDataStream& in, TQByteArray& array );
    void unpackCString( const UibStrTable& strings, TQDataStream& in,
			TQCString& cstr );
    void unpackString( const UibStrTable& strings, TQDataStream& in,
		       TQString& str );
    void unpackStringSplit( const UibStrTable& strings, TQDataStream& in,
			    TQString& str );
    void unpackVariant( const UibStrTable& strings, TQDataStream& in,
			TQVariant& value );
    void inputSpacer( const UibStrTable& strings, TQDataStream& in,
		      TQLayout *parent );
    void inputColumnOrRow( const UibStrTable& strings, TQDataStream& in,
			   TQObject *parent, bool isRow );
    void inputItem( const UibStrTable& strings, TQDataStream& in,
		    TQObject *parent, TQListViewItem *parentItem = 0 );
    void inputMenuItem( TQObject **objects, const UibStrTable& strings,
			TQDataStream& in, TQMenuBar *menuBar );
    TQObject *inputObject( TQObject **objects, int& numObjects,
			  const UibStrTable& strings, TQDataStream& in,
			  TQWidget *ancestorWidget, TQObject *parent,
			  TQCString className = "" );
    TQWidget *createFromUiFile( TQDomDocument doc, TQObject *connector,
			       TQWidget *parent, const char *name );
    TQWidget *createFromUibFile( TQDataStream& in, TQObject *connector,
				TQWidget *parent, const char *name );

private:
    struct Image {
	TQImage img;
	TQString name;
	bool operator==(  const Image &i ) const {
	    return ( i.name == name &&
		     i.img == img );
	}
    };

    struct Field
    {
	Field() {}
	Field( const TQString &s1, const TQPixmap &p, const TQString &s2 ) : name( s1 ), pix( p ), field( s2 ) {}
	TQString name;
	TQPixmap pix;
	TQString field;
    };

    struct SqlWidgetConnection
    {
	SqlWidgetConnection() {}
	SqlWidgetConnection( const TQString &c, const TQString &t )
	    : conn( c ), table( t ), dbControls( new TQMap<TQString, TQString>() ) {}
	TQString conn;
	TQString table;
	TQMap<TQString, TQString> *dbControls;
    };

    TQValueList<Image> images;
    TQWidget *toplevel;
    TQWidgetFactoryPrivate *d;
    TQMap<TQString, TQString> *dbControls;
    TQMap<TQString, TQStringList> dbTables;
    TQMap<TQWidget*, SqlWidgetConnection> sqlWidgetConnections;
    TQMap<TQString, TQString> buddies;
    TQMap<TQTable*, TQValueList<Field> > fieldMaps;
    TQPtrList<TQAction> actionList;
    TQMap<TQString, TQString> languageSlots;
    TQStringList noDatabaseWidgets;
    bool usePixmapCollection;
    int defMargin;
    int defSpacing;
    TQString code;
    TQString uiFileVersion;
};

#endif
